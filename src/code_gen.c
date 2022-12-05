#include "code_gen.h"
#include "error-codes.h"
#include "symtable.h"
#include "code_gen_static.h"
#include <stdlib.h>

enum ifjErrCode errorCode;

/// AUXILIARY FUNCTIONS

code_block *codeBlockConst(code_block_type type, codeGenCtx *ctx) {
    code_block *new = (code_block *) malloc(sizeof(code_block));
    CHECK_MALLOC_PTR(new);

    new->type = type;
    switch (type) {
        case BLOCK_IF:
            new->labelNum = ctx->if_count;
            ctx->if_count++;
            break;

        case BLOCK_ELSE:
            new->labelNum = ctx->else_number;
            break;

        case BLOCK_WHILE:
            new->labelNum = ctx->while_count;
            ctx->while_count++;
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

#define PUSH_FNC_DECL() stack_code_block_push(&ctx->block_stack, codeBlockConst(BLOCK_DECLARE, ctx))
#define PUSH_IF() stack_code_block_push(&ctx->block_stack, codeBlockConst(BLOCK_IF, ctx))
#define PUSH_ELSE() stack_code_block_push(&ctx->block_stack, codeBlockConst(BLOCK_ELSE, ctx))
#define PUSH_WHILE() stack_code_block_push(&ctx->block_stack, codeBlockConst(BLOCK_WHILE, ctx))

void code_generator(stack_ast_t *ast, ht_table_t *var_symtable) {
    if (stack_ast_empty(ast)) return;
    // generate built in functions
    genBuiltIns();

    // generate variable definitions in local frame for whole program
    INST_CREATEFRAME();
    INST_PUSHFRAME();
    genVarDefs(var_symtable, NULL);

    // allocate code context
    codeGenCtx *ctx = (codeGenCtx *) malloc(sizeof(codeGenCtx));
    CHECK_MALLOC(ctx);

    // init ctx
    ctx->if_count = 0;
    ctx->else_number = 0;
    ctx->while_count = 0;
    ctx->current_fnc_declaration = NULL;
    stack_code_block_init(&ctx->block_stack);

    AST_item *tmp = NULL; // prevent looping on error in the code gen
    while (!stack_ast_empty(ast)) {

        COMMENT("NEW EXPRESSION IN CODE GEN");

        if (errorCode != SUCCESS)
            break;

        // check if the item generated properly (is poped) to prevent looping
        if (AST_TOP() == tmp) {
            ERR_INTERNAL(code_generator, "top of AST stack is same as last generated item\n");
            break;
        }
        tmp = AST_TOP();

        switch (tmp->type) {
            case AST_IF:
                COMMENT("GENERATING IF");
                PUSH_IF();
                POP_AND_CALL(gen_condition);
                COMMENT("IF GENERATED");
                break;

            case AST_WHILE:
                COMMENT("GENERATING WHILE");
                PUSH_WHILE();
                INST_LABEL(LABEL_WHILE_BEGIN());
                POP_AND_CALL(gen_condition);
                COMMENT("WHILE GENERATED");
                break;

            case AST_ELSE:
                COMMENT("GENERATING ELSE");
                PUSH_ELSE();
                INST_LABEL(LABEL_ELSE());
                AST_POP();
                COMMENT("ELSE GENERATED");
                break;

            case AST_ASSIGN:
                COMMENT("GENERATING ASSIGN");
                AST_POP();
                gen_assign(ast);
                COMMENT("ASSIGN GENERATED");
                break;

            case AST_FUNCTION_CALL:
                COMMENT("GENERATING FUNCTION CALL");
                gen_fnc_call(ast);
                INST_POPS(VAR_BLACKHOLE()); // dispose the return value
                if (AST_TOP()->type != AST_END_EXPRESSION) {
                    ERR_INTERNAL(code_generator, "function call must be ended with end expession\n");
                    break;
                }
                AST_POP();
                COMMENT("FUNCTION CALL GENERATED");
                break;

            case AST_FUNCTION_DECLARE:
                COMMENT("GENERATING FUNCTION DECLARE");
                PUSH_FNC_DECL();
                // update ctx
                ctx->current_fnc_declaration = AST_TOP()->data->function_declare_data->function;

                // jump over function
                INST_JUMP(LABEL_FNC_DECLARE_END());

                INST_LABEL(LABEL(AST_TOP()->data->function_declare_data->function->identifier));
                // define vars in local frame (frame is created on function call)
                genVarDefs(AST_TOP()->data->function_declare_data->var_symtable, AST_TOP()->data->function_declare_data->function);
                AST_POP();
                COMMENT("FUNCTION DECLARE GENERATED");
                break;

            case AST_RETURN_VOID:
            case AST_RETURN_EXPR:
                COMMENT("GENERATING RETURN");
                gen_return(ast, ctx);
                COMMENT("RETURN GENERATED");
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
                ERR_INTERNAL(code_generator, "empty operation on top of ast\n");
                break;
            // begining of postfix expression
			case AST_VAR:
			case AST_INT:
			case AST_STRING:
			case AST_FLOAT:
			case AST_NULL:
                COMMENT("GENERATING EXPRESSION");
                gen_expr(ast); // generate expression, but do nothing with the data
                COMMENT("EXPRESSION GENERATED");
				break;

            case AST_END_BLOCK:
                COMMENT("GENERATING BLOCK END");
                if (stack_code_block_empty(&ctx->block_stack)) {
                    ERR_INTERNAL(code_generator, "block ended, but code block stack is empty\n");
                    break;
                }

                switch (stack_code_block_top(&ctx->block_stack)->type) {
                    case BLOCK_IF:
#if DEBUG == 1          // after if block has to be else
                        if (ast->top->next == NULL) {
                            ERR_INTERNAL(code_generator, "if block ended, next item should be else, but is NULL\n");
                        }
                        else if (ast->top->next->data->type != AST_ELSE) {
                            ERR_INTERNAL(code_generator, "if block ended, next item should be else, but is not\n");
                            printAstStack(ast);
                        }
#endif
                        INST_JUMP(LABEL_ENDELSE());
                        // set else number
                        ctx->else_number = stack_code_block_top(&ctx->block_stack)->labelNum;
                        break;

                    case BLOCK_ELSE:
                        INST_LABEL(LABEL_ENDELSE());
                        break;

                    case BLOCK_WHILE:
                        INST_JUMP(LABEL_WHILE_BEGIN());
                        INST_LABEL(LABEL_WHILE_END());
                        break;

                    case BLOCK_DECLARE:
                        if (ctx->current_fnc_declaration->fnc_data.returnType == void_t) {
                            // generate return for void function
                            INST_PUSHS(CONST_NIL());
                            INST_RETURN();
                        } else {
                            INST_JUMP(LABEL("no%return"));
                        }

                        // make end function label
                        INST_LABEL(LABEL_FNC_DECLARE_END());

                        ctx->current_fnc_declaration = NULL;
                        break;

                    default:
                        ERR_INTERNAL(code_generator, "unknown code block type\n");
                        break;
                }

                stack_code_block_pop(&ctx->block_stack); // pop current code block
                AST_POP(); // pop block end item
                COMMENT("BLOCK END GENERATED");
                break;

                default:
                    ERR_INTERNAL(code_generator, "unknown AST type. Type number: %d\n",tmp->type);
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

    if (!stack_code_block_empty(&ctx->block_stack)) {
        ERR_INTERNAL(code_generator, "code block stack not empty\n");
        printCodeBlockStack(&ctx->block_stack);
        while (!stack_code_block_empty(&ctx->block_stack))
            stack_code_block_pop(&ctx->block_stack);
    }

    if (errorCode == SUCCESS) {
        INST_EXIT(CONST_INT(0));
    }

    free(ctx);
}

void gen_assign(stack_ast_t *ast) {
    if (AST_TOP()->type != AST_VAR)
        ERR_INTERNAL(gen_assign,"Attempt to assign to a non variable type.");

    char *idenitfier = (char *) malloc((strlen(AST_TOP()->data->variable->identifier) + 1) * sizeof(char)); //save variable name
    CHECK_MALLOC(idenitfier);

    strcpy(idenitfier,AST_TOP()->data->variable->identifier);
    AST_POP(); //pop AST_VAR

    gen_expr(ast);
    INST_POPS(VAR_CODE("LF",idenitfier));//pop value into variable

    free(idenitfier);
}

void gen_expr(stack_ast_t *ast) {
    AST_item *item = AST_TOP();
    if (item->type == AST_END_EXPRESSION) //empty expression
        debug_log("gen_expr - empty expression\n");

    while (item->type != AST_END_EXPRESSION){
        switch (item->type)
        {
        // <literals>
        case AST_INT:
            INST_PUSHS(CONST_INT(item->data->int_value));
            break;

        case AST_FLOAT:
            INST_PUSHS(CONST_FLOAT(item->data->float_value));
            break;

        case AST_STRING:
            INST_PUSHS(CONST_STRING(item->data->string_value));
            break;

        case AST_NULL:
            INST_PUSHS(CONST_NIL());
            break;

        case AST_VAR:
            CHECK_INIT(VAR_CODE("LF",item->data->variable->identifier));
            INST_PUSHS(VAR_CODE("LF",item->data->variable->identifier));
            break;

        case AST_FUNCTION_CALL:
            gen_fnc_call(ast);
            item =AST_TOP();//show next
            continue;

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
            INST_CONCAT(VAR_AUX(1),VAR_AUX(2),VAR_AUX(1));
            INST_PUSHS(VAR_AUX(1));
            INST_POPFRAME();
            break;

        case AST_EQUAL:
        case AST_NOT_EQUAL:
            INST_CALL(LABEL("type%cmp"));//2 oprands already on stack
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
            ERR_INTERNAL(gen_expr, "Unexpected item type in expression. Item type: %d\n",item->type);
            return;
        }

    AST_POP();//pop current
    item = AST_TOP();//show next
    }

    if (AST_TOP()->type != AST_END_EXPRESSION) {
        ERR_INTERNAL(gen_expr, "wrong end of expression");
        return;
    }

    AST_POP();//pop AST_END_EXPR
}

void gen_fnc_call(stack_ast_t *ast) {
    // check for write
    if (!strcmp(AST_TOP()->data->function_call_data->function->identifier, "write")) {
        gen_write(ast);
        return;
    }

    INST_CREATEFRAME(); // new TF frame for function

    AST_fnc_param *param = AST_TOP()->data->function_call_data->params;
    param_info_t *ref = AST_TOP()->data->function_call_data->function->fnc_data.params;
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
                ERR_INTERNAL(gen_return,"Unexpected return type of function.\n");
                break;
            }
            INST_POPS(VAR_CODE("TF",ref->varId)); //pop to variable
            break; //case AST_P_VAR

        case AST_P_INT:
            INST_MOVE(VAR_CODE("TF",ref->varId),CONST_INT(param->data->int_value));
            break;
        case AST_P_STRING:
            INST_MOVE(VAR_CODE("TF",ref->varId),CONST_STRING(param->data->string_value));
            break;
        case AST_P_FLOAT:
            INST_MOVE(VAR_CODE("TF",ref->varId),CONST_FLOAT(param->data->float_value));
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
    INST_CALL(LABEL(AST_TOP()->data->function_call_data->function->identifier));
    INST_POPFRAME();
    AST_POP();
}

void gen_write(stack_ast_t *ast) {
    AST_fnc_param *tmpParam = AST_TOP()->data->function_call_data->params;

    while (tmpParam != NULL) {
        switch (tmpParam->type) {
            case AST_P_INT:
                INST_WRITE(CONST_INT(tmpParam->data->int_value));
                break;
            case AST_P_FLOAT:
                INST_WRITE(CONST_FLOAT(tmpParam->data->float_value));
                break;
            case AST_P_STRING:
                INST_WRITE(CONST_STRING(tmpParam->data->string_value));
                break;
            case AST_P_NULL:
                INST_WRITE(CONST_STRING(""));
                break;
            case AST_P_VAR:
                CHECK_INIT(VAR_CODE("LF", tmpParam->data->variable->identifier));
                INST_WRITE(VAR_CODE("LF", tmpParam->data->variable->identifier));
                break;
            default:
                ERR_INTERNAL(gen_write, "not recognised parameter type");
                break;
        }
        tmpParam = tmpParam->next;
    }

    // push return value to stack
    INST_PUSHS(CONST_NIL());
    AST_POP();
}


void gen_condition(CODE_GEN_PARAMS) {
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
            ERR_INTERNAL(gen_condition, "empty operation on top of ast");
            break;
		case AST_VAR:
		case AST_INT:
		case AST_STRING:
		case AST_FLOAT:
		case AST_NULL:
            gen_expr(ast); // gen expression, result will be on stack
            break;

        default:
            ERR_INTERNAL(genIf, "not recognized type on top of stack - %d\n", AST_TOP()->type);
            return;
    }

    // non bool value jumps - value is on top of stack
    INST_CALL(LABEL("resolve%condition")); // call resolve condition

    // push true to stack for comparison
    INST_PUSHS(CONST_BOOL("true"));
    switch (stack_code_block_top(&ctx->block_stack)->type) {
        case BLOCK_IF:
            INST_JUMPIFNEQS(LABEL_ELSE()); // if - cond false jump to else
            break;
        case BLOCK_WHILE:
            INST_JUMPIFNEQS(LABEL_WHILE_END()); // while - cond false jump to endwhile
            break;
        default:
            ERR_INTERNAL(gen_condition, "block stack is neither if or while");
            return;
    }
}

void gen_string(char *str) {
    if (str == NULL) {ERR_INTERNAL(gen_string, "NULL string pointer"); return;}
    printf("string@");
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] <= 32 || str[i] == 35 || str[i] == 92) {
            printf("\\%03d", str[i]);
        } else {
            printf("%c", str[i]);
        }
    }
}


void gen_return(CODE_GEN_PARAMS) {
    //main body return
    if (ctx->current_fnc_declaration == NULL){
        if(AST_TOP()->type == AST_RETURN_EXPR)
        {
            AST_POP();//pop AST_RETURN
            gen_expr(ast);
        }
        else
            AST_POP();//pop AST_RETURN
        INST_CLEARS();
        INST_POPFRAME();
        INST_EXIT(CONST_INT(0));
    }
    //function return
    else if (AST_TOP()->type == AST_RETURN_VOID){
        AST_POP();//pop AST_RETURN
        if (ctx->current_fnc_declaration->fnc_data.returnType != void_t)
            ERR_INTERNAL(gen_return,"This should not have gotten through syn analyzer\n");
        INST_PUSHS(CONST_NIL());//function return void
        INST_RETURN();
    }
    else { //AST_RETURN_EXPR
        AST_POP();//pop AST_RETURN
        gen_expr(ast);//gets result of expression on stack
        //compare return type of function vs type of expression
        switch (ctx->current_fnc_declaration->fnc_data.returnType)
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
            ERR_INTERNAL(gen_return,"Unexpected return type of function.\n");
            break;
        }
        //leaves result of expr on stack
        INST_RETURN(); //return from function
    }
}

