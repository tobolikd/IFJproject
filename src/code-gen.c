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

#define BLOCK_STACK_TOP_TYPE stack_code_block_top(&ctx->blockStack)->type

void codeGenerator(stack_ast_t *ast) {
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

    while (!stack_ast_empty(ast)) {
        if (errorCode != SUCCESS)
            break;

        switch (stack_ast_top(ast)->type) {
            case AST_IF:
                PUSH_IF();
                POP_AND_CALL(genIF);
                break;

            case AST_WHILE:
                PUSH_WHILE();
                PRINT_WHILE_BEGIN_LABEL();
                POP_AND_CALL(genWhile);
                break;

            case AST_ELSE:
                PUSH_ELSE();
                PRINT_ELSE_LABEL();
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

            case AST_RETURN:
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
                        PRINT_ENDIF_LABEL();
                        break;

                    case BLOCK_ELSE:
                        PRINT_ENDELSE_LABEL();
                        break;

                    case BLOCK_WHILE:
                        PRINT_WHILE_END_LABEL();
                        break;

                    case BLOCK_DECLARE:
                        PRINT_RETURN();
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

    // if there ade items on either stack some error occured during code generation
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

void genIF(CODE_GEN_PARAMS) {

}


void genWhile(CODE_GEN_PARAMS) {

}


void genString(CODE_GEN_PARAMS) {

}


void genReturn(CODE_GEN_PARAMS) {

}


void genBuiltIns() {

}



