/* @file code_gen.c
 *
 * @brief implementation of code generation
 *
 * @author David Tobolik (xtobol06)
 */

#include "code_gen.h"
#include "error_codes.h"
#include "symtable.h"
#include "code_gen_static.h"
#include <stdlib.h>

enum ifj_err_code error_code;

/// AUXILIARY FUNCTIONS
// construct code block from type and ctx including mem allocation
code_block *code_block_const(code_block_type type, code_gen_ctx_t *ctx) {
    code_block *new = (code_block *) malloc(sizeof(code_block));
    CHECK_MALLOC_PTR(new);

    new->type = type;

    // assign id number from context and increment it
    switch (type) {
        case BLOCK_IF:
            new->label_num = ctx->if_count;
            ctx->if_count++;
            break;

        case BLOCK_ELSE:
            new->label_num = ctx->else_number;
            break;

        case BLOCK_WHILE:
            new->label_num = ctx->while_count;
            ctx->while_count++;
            break;

        case BLOCK_DECLARE: // not needed for fnc declaration
            new->label_num = -1;
            break;

        default:
            ERR_INTERNAL(code_block_const, "unknown code block type\n");
            break;
    }

    return new;
}

/// !AUXILIARY FUNCTIONS

// macro shortcuts for constructing and pushing code blocks to nest stack
#define PUSH_FNC_DECL() stack_code_block_push(&ctx->block_stack, code_block_const(BLOCK_DECLARE, ctx))
#define PUSH_IF() stack_code_block_push(&ctx->block_stack, code_block_const(BLOCK_IF, ctx))
#define PUSH_ELSE() stack_code_block_push(&ctx->block_stack, code_block_const(BLOCK_ELSE, ctx))
#define PUSH_WHILE() stack_code_block_push(&ctx->block_stack, code_block_const(BLOCK_WHILE, ctx))

void code_generator(stack_ast_t *ast, ht_table_t *var_symtable) {
    if (stack_ast_empty(ast)) return;
    // generate built in functions
    gen_static();

    // generate variable definitions in local frame for whole program
    INST_CREATEFRAME();
    INST_PUSHFRAME();
    gen_var_definitions(var_symtable, NULL);

    // allocate code context
    code_gen_ctx_t *ctx = (code_gen_ctx_t *) malloc(sizeof(code_gen_ctx_t));
    CHECK_MALLOC(ctx);

    // init ctx
    ctx->if_count = 0;
    ctx->else_number = 0;
    ctx->while_count = 0;
    ctx->current_fnc_declaration = NULL;
    stack_code_block_init(&ctx->block_stack);

    // prevent looping on error in the code gen
    AST_item *tmp = NULL;

    while (!stack_ast_empty(ast)) {
        // on error end generating
        if (error_code != SUCCESS)
            break;

        // check if the item generated properly (is poped) to prevent looping
        if (AST_TOP() == tmp) {
            ERR_INTERNAL(code_generator, "top of AST stack is same as last generated item\n");
            break;
        }
        tmp = AST_TOP();

        // according to ast top type, generate code
        switch (tmp->type) {
            case AST_IF:
                PUSH_IF();  // add if to nest stack
                AST_POP();  // pop if item
                gen_condition(ast, ctx);    // generate condition
                break;

            case AST_WHILE:
                PUSH_WHILE();   // add while to nest stack
                INST_LABEL(LABEL_WHILE_BEGIN());    // gen while begin label
                AST_POP();  // pop while item
                gen_condition(ast, ctx); // generate condition
                break;

            case AST_ELSE:
                PUSH_ELSE();    // add else to nest stack
                INST_LABEL(LABEL_ELSE());   // gen else label
                AST_POP();  // pop else item
                break;

            case AST_ASSIGN:
                AST_POP();  // pop assign item
                gen_assign(ast);    // generate assign
                break;

            case AST_FUNCTION_CALL:
                gen_fnc_call(ast);  // generate function call
                INST_POPS(VAR_BLACKHOLE()); // dispose the return value

                if (AST_TOP()->type != AST_END_EXPRESSION) {
                    ERR_INTERNAL(code_generator, "function call must be ended with end expession\n");
                    break;
                }

                AST_POP();  // pop end expression
                break;

            case AST_FUNCTION_DECLARE:
                PUSH_FNC_DECL(); // add function declaration to nest stack
                // update ctx
                ctx->current_fnc_declaration = AST_TOP()->data->function_declare_data->function;
                // jump over function if not called
                INST_JUMP(LABEL_FNC_DECLARE_END());
                INST_LABEL(LABEL(ctx->current_fnc_declaration->identifier));
                // define vars in local frame (frame is created on function call)
                gen_var_definitions(AST_TOP()->data->function_declare_data->var_symtable, ctx->current_fnc_declaration);
                AST_POP();  // pop function declare 
                break;

            case AST_RETURN_VOID:
            case AST_RETURN_EXPR:
                gen_return(ast, ctx);
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
                gen_expr(ast); // generate expression, but do nothing with the data
				break;

            case AST_END_BLOCK:
                if (stack_code_block_empty(&ctx->block_stack)) {
                    ERR_INTERNAL(code_generator, "block ended, but code block stack is empty\n");
                    break;
                }
                // according to block type, generate end
                switch (stack_code_block_top(&ctx->block_stack)->type) {
                    case BLOCK_IF:
                        // on if block end, skip else block
                        INST_JUMP(LABEL_ENDELSE());
                        // set else number to be the same as this if number
                        ctx->else_number = stack_code_block_top(&ctx->block_stack)->label_num;
                        break;

                    case BLOCK_ELSE:
                        // label else end
                        INST_LABEL(LABEL_ENDELSE());
                        break;

                    case BLOCK_WHILE:
                        // on while end, jump to while start
                        INST_JUMP(LABEL_WHILE_BEGIN());
                        // while end label
                        INST_LABEL(LABEL_WHILE_END());
                        break;

                    case BLOCK_DECLARE:
                        if (ctx->current_fnc_declaration->fnc_data.returnType == void_t) {
                            // generate return for void function
                            INST_PUSHS(CONST_NIL());
                            INST_RETURN();
                        } else {
                            // if function wasnt returned, go to error label
                            INST_JUMP(LABEL("no%return"));
                        }

                        // make end function label
                        INST_LABEL(LABEL_FNC_DECLARE_END());
                        // delete curr function pointer
                        ctx->current_fnc_declaration = NULL;
                        break;

                    default:
                        ERR_INTERNAL(code_generator, "unknown code block type\n");
                        break;
                }
                stack_code_block_pop(&ctx->block_stack); // pop current code block
                AST_POP(); // pop block end item
                break;
                // !AST_END_BLOCK

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
        print_code_block_stack(&ctx->block_stack);
        while (!stack_code_block_empty(&ctx->block_stack))
            stack_code_block_pop(&ctx->block_stack);
    }

    if (error_code == SUCCESS) {
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

/* gen_expr
 *
 * Generates expression. Resolves expression using stack method.
 * Semantically checks validity of used variables.
 * 
 * @author Gabriel Biel (xbielg00)
 */
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
            INST_POPS(AUX1);
            INST_POPS(AUX2);
            INST_CONCAT(AUX1, AUX2, AUX1);
            INST_PUSHS(AUX1);
            break;

        case AST_EQUAL:
        case AST_NOT_EQUAL:
            INST_CALL(LABEL("math%equal"));//2 oprands already on stack
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

/* gen_fnc_call
 *
 * Generates function call. Calls label of the function which is to be called.
 * Semantically checks parameter types.
 * 
 * @author Gabriel Biel (xbielg00)
 */
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

    // go through all params and write them
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
    
    // generaete expression (if stack is valid)
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
        // check for every char if it can be printed normally or using escape sequence
        if (str[i] <= 32 || str[i] == 35 || str[i] == 92) {
            printf("\\%03d", str[i]);
        } else {
            printf("%c", str[i]);
        }
    }
}

/* gen_return
 *
 * Generates proper action when return is called.
 * Semantically checks return value - type while in function.
 * 
 * @author Gabriel Biel (xbielg00)
 */
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

