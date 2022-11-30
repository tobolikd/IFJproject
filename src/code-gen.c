#include "code-gen.h"
#include "error-codes.h"
#include "symtable.h"
#include "code-gen-data.h"
#include <stdlib.h>

enum ifjErrCode errorCode;

/// AUXILIARY FUNCTIONS

code_block *codeBlockConst(code_block_type type, codeGenCtx *ctx) {
    code_block *new = (code_block *) malloc(sizeof(code_block));
    CHECK_MALLOC_PTR(new);

    new->type = type;
    switch (type) {
        case BLOCK_IF:
            new->labelNum = ctx->ifCount;
            ctx->ifCount++;
            break;

        case BLOCK_ELSE:
            new->labelNum = ctx->elseNum;
            break;

        case BLOCK_WHILE:
            new->labelNum = ctx->whileCount;
            ctx->whileCount++;
            break;

        case BLOCK_DECLARE:
            new->labelNum = -1;
            break;

        default:
            ERR_INTERNAL(codeBlockConst, "unknown code block type\n");
            break;
    }

    return new;
}

/// !AUXILIARY FUNCTIONS

#define POP_AND_CALL(FNC) do { AST_POP(); FNC(ast, ctx); } while (0)

#define PUSH_FNC_DECL() stack_code_block_push(&ctx->blockStack, codeBlockConst(BLOCK_DECLARE, ctx))
#define PUSH_IF() stack_code_block_push(&ctx->blockStack, codeBlockConst(BLOCK_IF, ctx))
#define PUSH_ELSE() stack_code_block_push(&ctx->blockStack, codeBlockConst(BLOCK_ELSE, ctx))
#define PUSH_WHILE() stack_code_block_push(&ctx->blockStack, codeBlockConst(BLOCK_WHILE, ctx))

void codeGenerator(stack_ast_t *ast, ht_table_t *varSymtable) {
    if (stack_ast_empty(ast)) return;
    // generate built in functions
    genBuiltIns();

    // generate variable definitions in local frame for whole program
    INST_CREATEFRAME();
    INST_PUSHFRAME();
    genVarDefs(varSymtable, NULL);

    // allocate code context
    codeGenCtx *ctx = (codeGenCtx *) malloc(sizeof(codeGenCtx));
    CHECK_MALLOC(ctx);

    // init ctx
    ctx->auxCount = 0;
    ctx->ifCount = 0;
    ctx->elseNum = 0;
    ctx->whileCount = 0;
    ctx->currentFncDeclaration = NULL;
    stack_code_block_init(&ctx->blockStack);

    AST_item *tmp = NULL; // prevent looping on error in the code gen
    while (!stack_ast_empty(ast)) {
        if (errorCode != SUCCESS)
            break;

        // check if the item generated properly (is poped) to prevent looping
        if (AST_TOP() == tmp) {
            ERR_INTERNAL(codeGenerator, "top of AST stack is same as last generated item\n");
            break;
        }
        tmp = AST_TOP();

        switch (tmp->type) {
            case AST_IF:
                PUSH_IF();
                POP_AND_CALL(genCondition);
                break;

            case AST_WHILE:
                PUSH_WHILE();
                INST_LABEL(LABEL_WHILE_BEGIN());
                POP_AND_CALL(genCondition);
                break;

            case AST_ELSE:
                PUSH_ELSE();
                INST_LABEL(LABEL_ELSE());
                AST_POP();
                break;

            case AST_ASSIGN:
                AST_POP();
                genAssign(ast);
                break;

            case AST_FUNCTION_CALL:
                genFncCall(ast);
                INST_POPS(VAR_BLACKHOLE()); // dispose the return value
                break;

            case AST_FUNCTION_DECLARE:
                PUSH_FNC_DECL();
                // update ctx
                ctx->currentFncDeclaration = AST_TOP()->data->functionDeclareData->function;
                INST_LABEL(LABEL(AST_TOP()->data->functionDeclareData->function->identifier));
                // define vars in local frame (frame is created on function call)
                genVarDefs(AST_TOP()->data->functionDeclareData->varSymtable, AST_TOP()->data->functionDeclareData->function);
                AST_POP();
                break;

            case AST_RETURN_VOID:
            case AST_RETURN_EXPR:
                genReturn(ast, ctx);
                break;

            // expressions are postfix
            // cant have empty expression with just operator
            case AST_ADD:
			case AST_SUBTRACT:
			case AST_DIVIDE:
			case AST_MULTIPLY:
			case AST_CONCAT:
			case AST_EQUAL:
			case AST_NOT_EQUAL:
			case AST_GREATER:
			case AST_GREATER_EQUAL:
			case AST_LESS:
			case AST_LESS_EQUAL:
            case AST_END_EXPRESSION:
                ERR_INTERNAL(codeGenerator, "empty operation on top of ast");
                break;
            // begining of postfix expression
			case AST_VAR:
			case AST_INT:
			case AST_STRING:
			case AST_FLOAT:
			case AST_NULL:
                genExpr(ast); // generate expression, but do nothing with the data
				break;

            case AST_END_BLOCK:
                if (stack_code_block_empty(&ctx->blockStack)) {
                    ERR_INTERNAL(codeGenerator, "block ended, but code block stack is empty\n");
                    break;
                }

                switch (stack_code_block_top(&ctx->blockStack)->type) {
                    case BLOCK_IF:
#if DEBUG == 1          // after if block has to be else
                        if (ast->top->next == NULL) {
                            ERR_INTERNAL(codeGenerator, "if block ended, next item should be else, but is NULL\n");
                        }
                        else if (ast->top->next->data->type != AST_ELSE) {
                            ERR_INTERNAL(codeGenerator, "if block ended, next item should be else, but is not\n");
                            printAstStack(ast);
                        }
#endif
                        INST_JUMP(LABEL_ENDELSE());
                        // set else number
                        ctx->elseNum = stack_code_block_top(&ctx->blockStack)->labelNum;
                        break;

                    case BLOCK_ELSE:
                        INST_LABEL(LABEL_ENDELSE());
                        break;

                    case BLOCK_WHILE:
                        INST_JUMP(LABEL_WHILE_BEGIN());
                        INST_LABEL(LABEL_WHILE_END());
                        break;

                    case BLOCK_DECLARE:
                        if (ctx->currentFncDeclaration->fnc_data.returnType == void_t) {
                            // generate return for void function
                            INST_PUSHS(CONST_NIL());
                            INST_RETURN();
                        }
                        ctx->currentFncDeclaration = NULL;
                        break;

                    default:
                        ERR_INTERNAL(codeGenerator, "unknown code block type\n");
                        break;
                }

                stack_code_block_pop(&ctx->blockStack); // pop current code block
                AST_POP(); // pop block end item
                break;

                default:
                    ERR_INTERNAL(codeGenerator, "unknown AST type. Type number: %d\n",tmp->type);
                    break;
        }
    }

    // if there are items on either stack some error occured during code generation
    // print debug info
    if (!stack_ast_empty(ast)) {
        ERR_AST_NOT_EMPTY(ast);
        while (!stack_ast_empty(ast))
            AST_POP();
    }

    if (!stack_code_block_empty(&ctx->blockStack)) {
        ERR_INTERNAL(codeGenerator, "code block stack not empty\n");
        printCodeBlockStack(&ctx->blockStack);
        while (!stack_code_block_empty(&ctx->blockStack))
            stack_code_block_pop(&ctx->blockStack);
    }

    free(ctx);
}

void genAssign(stack_ast_t *ast) {
    if (AST_TOP()->type != AST_VAR)
        ERR_INTERNAL(genAssign,"Attempt to assign to a non variable type.");

    char *idenitfier = (char *) malloc((strlen(AST_TOP()->data->variable->identifier) + 1) * sizeof(char)); //save variable name
    CHECK_MALLOC(idenitfier);

    strcpy(idenitfier,AST_TOP()->data->variable->identifier);
    AST_POP(); //pop AST_VAR

    genExpr(ast);
    INST_POPS(VAR_CODE("LF",idenitfier));//pop value into variable

    free(idenitfier);
}

void genExpr(stack_ast_t *ast) {
    AST_item *item = AST_TOP();
    if (item->type == AST_END_EXPRESSION) //empty expression
        debug_log("genExpr - empty expression\n");

    while (item->type != AST_END_EXPRESSION){
        switch (item->type)
        {
        // <literals>
        case AST_INT:
            INST_PUSHS(CONST_INT(item->data->intValue));
            break;

        case AST_FLOAT:
            INST_PUSHS(CONST_FLOAT(item->data->floatValue));
            break;

        case AST_STRING:
            INST_PUSHS(CONST_STRING(item->data->stringValue));
            break;

        case AST_VAR:
            CHECK_INIT(VAR_CODE("LF",item->data->variable->identifier));
            INST_PUSHS(VAR_CODE("LF",item->data->variable->identifier));
            break;

        case AST_FUNCTION_CALL:
            genFncCall(ast);
            break;

        case AST_ADD:
            INST_CALL(LABEL("conv%arithmetic"));
            INST_ADDS();
            break;

        case AST_SUBTRACT:
            INST_CALL(LABEL("conv%arithmetic"));
            INST_SUBS();
            break;

        case AST_MULTIPLY:
            INST_CALL(LABEL("conv%arithmetic"));
            INST_MULS();
            break;

        case AST_DIVIDE:
            INST_CALL(LABEL("conv%div"));
            INST_DIVS();
            break;

        case AST_CONCAT:
            //generate 3AC
            INST_CALL(LABEL("conv%concat"));
            INST_CREATEFRAME();
            INST_PUSHFRAME();
            INST_DEFVAR(VAR_AUX(1));
            INST_DEFVAR(VAR_AUX(2));
            INST_POPS(VAR_AUX(1));
            INST_POPS(VAR_AUX(2));
            INST_CONCAT(VAR_AUX(1),VAR_AUX(1),VAR_AUX(2));
            INST_PUSHS(VAR_AUX(1));
            INST_POPFRAME();
            break;

        case AST_EQUAL:
        case AST_NOT_EQUAL:
            INST_CALL(LABEL("type%cmp"));//2 oprands already on stack
            INST_POPS(AUX1);//bool result of type%cmp

            //if types are not the same skip evaluation of eq, push false
	        INST_JUMPIFNEQ(LABEL("incompatible%types"), AUX1, CONST_BOOL("true"));
            INST_EQS();
            INST_JUMP(LABEL("leave%eq"));//skip incompatible types

            INST_LABEL(LABEL("incompatible%types"));
            INST_POPS(VAR_BLACKHOLE());//pop operand
            INST_POPS(VAR_BLACKHOLE());//pop operand
            INST_PUSHS(CONST_BOOL("false"));

            INST_LABEL(LABEL("leave%eq"));
            if (item->type == AST_NOT_EQUAL)
                INST_NOTS();//negate the outcome

            break;

        case AST_LESS:
            INST_CALL(LABEL("conv%rel"));
            INST_LTS();
            break;

        case AST_LESS_EQUAL:
            INST_CALL(LABEL("conv%rel"));
            INST_GTS();
            INST_NOTS();
            break;

        case AST_GREATER:
            INST_CALL(LABEL("conv%rel"));
            INST_GTS();
            break;

        case AST_GREATER_EQUAL:
            INST_CALL(LABEL("conv%rel"));
            INST_LTS();
            INST_NOTS();
            break;

        case AST_END_EXPRESSION:
            continue;

        default:
            ERR_INTERNAL(genExpr, "Unexpected item type in expression. Item type: %d\n",item->type);
            return;
        }

    AST_POP();//pop current
    item =AST_TOP();//show next
    }
    AST_POP();//pop AST_END_EXPR

}

void genFncCall(stack_ast_t *ast) {
    // check for write
    if (!strcmp(AST_TOP()->data->functionCallData->function->identifier, "write")) {
        genWrite(ast);
        return;
    }

    INST_CREATEFRAME(); // new TF frame for function

    AST_fnc_param *param = AST_TOP()->data->functionCallData->params;
    param_info_t *ref = AST_TOP()->data->functionCallData->function->fnc_data.params;
    while (param != NULL)
    {
        INST_DEFVAR(VAR_CODE("TF",ref->varId)); //declare parameters as variables for in function use

        switch (param->type)
        {
        case AST_P_VAR: // check if variable type coresponds to fnc declare
                        // constats are dealt with in sem_analyser
            //push variable to stack
            CHECK_INIT(VAR_CODE("LF",param->data->variable->identifier));
            INST_PUSHS(VAR_CODE("LF",param->data->variable->identifier));
            switch (ref->type)
            {
            case int_t:
                INST_CALL(LABEL("type%check%int"));
                break;

            case float_t:
                INST_CALL(LABEL("type%check%float"));
                break;

            case string_t:
                INST_CALL(LABEL("type%check%string"));
                break;

            case null_int_t:
                INST_CALL(LABEL("type%check%int%nil"));
                break;

            case null_float_t:
                INST_CALL(LABEL("type%check%float%nil"));
                break;

            case null_string_t:
                INST_CALL(LABEL("type%check%string%nil"));
                break;

            default:
                ERR_INTERNAL(genReturn,"Unexpected return type of function.\n");
                break;
            }
            INST_POPS(VAR_CODE("TF",ref->varId)); //pop to variable
            break; //case AST_P_VAR

        case AST_P_INT:
            INST_MOVE(VAR_CODE("TF",ref->varId),CONST_INT(param->data->intValue));
            break;
        case AST_P_STRING:
            INST_MOVE(VAR_CODE("TF",ref->varId),CONST_FLOAT(param->data->floatValue));
            break;
        case AST_P_FLOAT:
            INST_MOVE(VAR_CODE("TF",ref->varId),CONST_STRING(param->data->stringValue));
            break;
        case AST_P_NULL: // allways false
            INST_MOVE(VAR_CODE("TF",ref->varId),CONST_NIL());
        default:
            break;
        }
        //read next param
        param = param->next;
        ref = ref->next;
    }
    //call function with its relevant frame
    INST_PUSHFRAME();
    INST_CALL(LABEL(AST_TOP()->data->functionCallData->function->identifier));
    INST_POPFRAME();
}

void genWrite(stack_ast_t *ast) {
    AST_fnc_param *tmpParam = AST_TOP()->data->functionCallData->params;

    while (tmpParam != NULL) {
        switch (tmpParam->type) {
            case AST_P_INT:
                INST_WRITE(CONST_INT(tmpParam->data->intValue));
                break;
            case AST_P_FLOAT:
                INST_WRITE(CONST_FLOAT(tmpParam->data->floatValue));
                break;
            case AST_P_STRING:
                INST_WRITE(CONST_STRING(tmpParam->data->stringValue));
                break;
            case AST_P_NULL:
                INST_WRITE(CONST_STRING(""));
                break;
            case AST_P_VAR:
                INST_WRITE(VAR_CODE("LF", tmpParam->data->variable->identifier));
                break;
            default:
                ERR_INTERNAL(genWrite, "not recognised parameter type");
                break;
        }
    }

    // push return value to stack
    INST_PUSHS(CONST_NIL());
    AST_POP();
}


void genCondition(CODE_GEN_PARAMS) {
    if (stack_ast_empty(ast)) { ERR_INTERNAL(genIf, "empty if condition\n"); return; }
    // for non bool expressions push result to stack
    // relational or null - generate jump directly
    switch (AST_TOP()->type) {
        case AST_ADD:
        case AST_SUBTRACT:
        case AST_DIVIDE:
        case AST_MULTIPLY:
        case AST_CONCAT:
        case AST_EQUAL:
        case AST_NOT_EQUAL:
        case AST_GREATER:
        case AST_GREATER_EQUAL:
        case AST_LESS:
        case AST_LESS_EQUAL:
        case AST_END_EXPRESSION:
            ERR_INTERNAL(genCondition, "empty operation on top of ast");
            break;
		case AST_VAR:
		case AST_INT:
		case AST_STRING:
		case AST_FLOAT:
		case AST_NULL:
            genExpr(ast); // gen expression, result will be on stack
            break;

        default:
            ERR_INTERNAL(genIf, "not recognized type on top of stack - %d\n", AST_TOP()->type);
            return;
    }

    // non bool value jumps - value is on top of stack
    INST_CALL(LABEL("resolve%condition")); // call resolve condition

    // push true to stack for comparison
    INST_PUSHS(CONST_BOOL("true"));
    switch (stack_code_block_top(&ctx->blockStack)->type) {
        case BLOCK_IF:
            INST_JUMPIFNEQS(LABEL_ELSE()); // if - cond false jump to else
            break;
        case BLOCK_WHILE:
            INST_JUMPIFNEQS(LABEL_WHILE_END()); // while - cond false jump to endwhile
            break;
        default:
            ERR_INTERNAL(genCondition, "block stack is neither if or while");
            return;
    }
}

void genString(char *str) {
    if (str == NULL) {ERR_INTERNAL(genString, "NULL string pointer"); return;}
    printf("string@");
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] <= 32 || str[i] == 35 || str[i] == 92) {
            printf("\\%03d", str[i]);
        } else {
            printf("%c", str[i]);
        }
    }
}


void genReturn(CODE_GEN_PARAMS) {
    //main body return
    if (ctx->currentFncDeclaration == NULL){
        AST_POP();//pop AST_RETURN
        INST_CLEARS();
        INST_POPFRAME();
        INST_EXIT(CONST_INT(0));
    }
    //function return
    else if (AST_TOP()->type == AST_RETURN_VOID){
        AST_POP();//pop AST_RETURN
        if (ctx->currentFncDeclaration->fnc_data.returnType != void_t)
            ERR_INTERNAL(genReturn,"This should not have gotten through syn analyzer\n");
        INST_PUSHS(CONST_NIL());//function return void
        INST_RETURN();
    }
    else { //AST_RETURN_EXPR
        AST_POP();//pop AST_RETURN
        genExpr(ast);//gets result of expression on stack
        //compare return type of function vs type of expression
        switch (ctx->currentFncDeclaration->fnc_data.returnType)
        {
        case int_t:
            INST_CALL(LABEL("type%check%int"));
            break;

        case float_t:
            INST_CALL(LABEL("type%check%float"));
            break;

        case string_t:
            INST_CALL(LABEL("type%check%string"));
            break;

        case null_int_t:
            INST_CALL(LABEL("type%check%int%nil"));
            break;

        case null_float_t:
            INST_CALL(LABEL("type%check%float%nil"));
            break;

        case null_string_t:
            INST_CALL(LABEL("type%check%string%nil"));
            break;

        default:
            ERR_INTERNAL(genReturn,"Unexpected return type of function.\n");
            break;
        }
        //leaves result of expr on stack
        INST_RETURN(); //return from function
    }
}

