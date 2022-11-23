#ifndef IFJ_CODE_GEN_H
#define IFJ_CODE_GEN_H

#include "ast.h"
#include "stack.h"
#include "code-gen-data.h"

typedef struct {
    unsigned auxCount;
    unsigned ifCount;
    unsigned elseCount;
    unsigned whileCount;
    ht_item_t *currentFncDeclaration;
    stack_code_block_t blockStack;
} codeGenCtx;

#define CODE_GEN_PARAMS stack_ast_t *ast, codeGenCtx *ctx

/* codeGenerator
 *
 * ast - AST created in syn-sem analysis
 */
void codeGenerator(stack_ast_t *ast);

void genAssign(CODE_GEN_PARAMS);

/* genExpr
 *
 * generates expression from ast
 * stores it in auxiliary variable
 * returns number of aux var where the result is stored
 */
int genExpr(CODE_GEN_PARAMS);

void genFncDeclare(CODE_GEN_PARAMS);

void genFncCall(CODE_GEN_PARAMS);

void genIF(CODE_GEN_PARAMS);

void genWhile(CODE_GEN_PARAMS);

void genString(CODE_GEN_PARAMS);

void genReturn(CODE_GEN_PARAMS);

void genBuiltIns();

#define PRINT_START() printf(".IFJcode22\n")

// IF ELSE
#define PRINT_IF_JMP(JUMP_INSTRUCTION) printf("%s else%%%d", #JUMP_INSTRUCTION, stack_code_block_top(&ctx->blockStack)->labelNum)

#define PRINT_ENDIF_LABEL() printf("JUMP end_else%%%d\n", stack_code_block_top(&ctx->blockStack)->labelNum)

#define PRINT_ELSE_LABEL() printf("LABEL else%%%d\n", stack_code_block_top(&ctx->blockStack)->labelNum)

#define PRINT_ENDELSE_LABEL() printf("LABEL end_else%%%d\n", stack_code_block_top(&ctx->blockStack)->labelNum)

// WHILE
#define PRINT_WHILE_BEGIN_LABEL() printf("LABEL while_begin%%%d\n", stack_code_block_top(&ctx->blockStack)->labelNum)

#define PRINT_WHILE_JMP(JUMP_INSTRUCTION) printf("%s while_end%%%d", #JUMP_INSTRUCTION, stack_code_block_top(&ctx->blockStack)->labelNum)

#define PRINT_WHILE_END_LABEL()                                                      \
    do {printf("JUMP while_begin%%%d\n", stack_code_block_top(&ctx->blockStack)->labelNum); \
        printf("LABEL while_end%%%d\n", stack_code_block_top(&ctx->blockStack)->labelNum);  \
    } while(0)

// DUNCTION DECLARE
#define PRINT_RETURN() printf("RETURN\n")

#endif // IFJ_CODE_GEN_H
