#include "code-gen.h"
#include "error-codes.h"
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
            new->labelNum = ctx->elseCount;
            ctx->elseCount++;
            break;

        case BLOCK_WHILE:
            new->labelNum = ctx->whileCount;
            ctx->whileCount++;
            break;

        case BLOCK_DECLARE:
            break;

        default:
            ERR_INTERNAL(codeBlockConst, "unknown code block type");
            break;
    }

    return new;
}

/// !AUXILIARY FUNCTIONS

#define POP_AND_CALL(FNC) do { stack_ast_pop(ast); FNC(ast, ctx); } while (0)

#define PUSH_IF() stack_code_block_push(&ctx->blockStack, codeBlockConst(BLOCK_IF, ctx))
#define PUSH_ELSE() stack_code_block_push(&ctx->blockStack, codeBlockConst(BLOCK_ELSE, ctx))
#define PUSH_WHILE() stack_code_block_push(&ctx->blockStack, codeBlockConst(BLOCK_WHILE, ctx))

void codeGenerator(stack_ast_t *ast) {
    if (stack_ast_empty(ast)) return;
    // generate built in functions
    genBuiltIns();

    // allocate code context
    codeGenCtx *ctx = (codeGenCtx *) malloc(sizeof(codeGenCtx));
    CHECK_MALLOC(ctx);

    // init ctx
    ctx->auxCount = 0;
    ctx->ifCount = 0;
    ctx->elseCount = 0;
    ctx->whileCount = 0;
    ctx->currentFncDeclaration = NULL;
    stack_code_block_init(&ctx->blockStack);

    AST_item *tmp = NULL; // prevent looping on error in the code gen
    while (!stack_ast_empty(ast)) {
        if (errorCode != SUCCESS)
            break;

        // check if the item generated properly (is poped) to prevent looping
        if (stack_ast_top(ast) == tmp) {
            ERR_INTERNAL(codeGenerator, "top of AST stack is same as last generated item");
            break;
        }

        tmp = stack_ast_top(ast);

        switch (tmp->type) {
            case AST_IF:
                PUSH_IF();
                POP_AND_CALL(genIf);
                break;

            case AST_WHILE:
                PUSH_WHILE();
                INST_LABEL(LABEL_WHILE_BEGIN());
                POP_AND_CALL(genWhile);
                break;

            case AST_ELSE:
                PUSH_ELSE();
                INST_LABEL(LABEL_ELSE());
                break;

            case AST_ASSIGN:
                POP_AND_CALL(genAssign);
                break;

            // following cases dont pop, because AST item contains useful data
            case AST_FUNCTION_CALL:
                genFncCall(ast, ctx);
                break;

            case AST_FUNCTION_DECLARE:
                genFncDeclare(ast, ctx);
                break;

            case AST_RETURN_VOID:
            case AST_RETURN_EXPR:
                genReturn(ast, ctx);
                break;

            // not assigned expression
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
			case AST_VAR:
			case AST_INT:
			case AST_STRING:
			case AST_FLOAT:
			case AST_NULL:
                genExpr(ast, ctx); // generate expression, but throw data away
				break;

            case AST_END_BLOCK:
                if (stack_code_block_empty(&ctx->blockStack)) {
                    ERR_INTERNAL(codeGenerator, "block ended, but code block stack is empty");
                    break;
                }

                switch (stack_code_block_top(&ctx->blockStack)->type) {
                    case BLOCK_IF:
#if DEBUG == 1          // after if block has to be else
                        if (ast->top->next->data->type != AST_ELSE) {
                            ERR_INTERNAL(codeGenerator, "if block ended, next item should be else, but is not");
                            printAstStack(ast);
                        }
#endif
                        INST_JUMP(LABEL_ENDELSE());
                        break;

                    case BLOCK_ELSE:
                        INST_LABEL(LABEL_ENDELSE());
                        break;

                    case BLOCK_WHILE:
                        INST_JUMP(LABEL_WHILE_BEGIN());
                        INST_LABEL(LABEL_WHILE_END());
                        break;

                    case BLOCK_DECLARE:
                        INST_RETURN(); // print return for case of void function
                                       // with no return statement at end of it
                        ctx->currentFncDeclaration = NULL;
                        break;

                    default:
                        ERR_INTERNAL(codeGenerator, "unknown code block type");
                        break;
                }
                stack_code_block_pop(&ctx->blockStack); // pop current code block
                stack_ast_pop(ast); // pop block end item
                break;

                default:
                    ERR_INTERNAL(codeGenerator, "unknown AST type");
                    break;
        }
    }

    // if there are items on either stack some error occured during code generation
    // print debug info
    if (!stack_ast_empty(ast)) {
        ERR_AST_NOT_EMPTY(ast);
        while (!stack_ast_empty(ast))
            stack_ast_pop(ast);
    }

    if (!stack_code_block_empty(&ctx->blockStack)) {
        ERR_INTERNAL(codeGenerator, "code block stack not empty");
        printCodeBlockStack(&ctx->blockStack);
        while (!stack_code_block_empty(&ctx->blockStack))
            stack_code_block_pop(&ctx->blockStack);
    }

    free(ctx);
}

void genAssign(CODE_GEN_PARAMS) {

}

int genExpr(CODE_GEN_PARAMS) {
    return 0;
}

void genFncDeclare(CODE_GEN_PARAMS) {

}

void genFncCall(CODE_GEN_PARAMS) {

}

void genIf(CODE_GEN_PARAMS) {
    switch (stack_ast_top(ast)->type) {
		case AST_ADD:
		case AST_SUBTRACT:
		case AST_DIVIDE:
		case AST_MULTIPLY:
		case AST_CONCAT:
            // expression
            INST_PUSHS(CONST_BOOL(true));
            INST_PUSHS(VAR_AUX(genExpr(ast, ctx))); // gen expression
                                                    // push result to stack
            INST_CALL(LABEL("resolve%%expr%%condition")); // call resolve
                                                          // result is on stack
            INST_JUMPIFNEQS(LABEL_ELSE()); // if result is false, jump to else
            break;
		case AST_EQUAL:

		case AST_NOT_EQUAL:
		case AST_GREATER:
		case AST_GREATER_EQUAL:
		case AST_LESS:
		case AST_LESS_EQUAL:

		case AST_VAR:
		case AST_INT:
		case AST_STRING:
		case AST_FLOAT:
            break;
		case AST_NULL:
            break;


        default:
            ERR_INTERNAL(genIf, "not recognized type on top of stack - %d", stack_ast_top(ast)->type);
    }
}


void genWhile(CODE_GEN_PARAMS) {

}


void genString(char *string) {

}


void genReturn(CODE_GEN_PARAMS) {

}


void genBuiltIns() {

}


