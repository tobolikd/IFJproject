#ifndef IFJ_CODE_GEN_H
#define IFJ_CODE_GEN_H 1

#include "ast.h"
#include "stack.h"
#include "code-gen-data.h"

typedef struct {
    // counts are used to generate unique labels and auxiliary variable names
    unsigned auxCount;
    unsigned ifCount;
    unsigned elseCount;
    unsigned whileCount;
    ht_item_t *currentFncDeclaration; // pointer to fnc symtable
                                      // NULL - in main body
                                      // ptr - in function declaration
    stack_code_block_t blockStack;  // stack of code blocks for nested blocks
} codeGenCtx;

#define CODE_GEN_PARAMS stack_ast_t *ast, codeGenCtx *ctx

/* codeGenerator
 *
 * ast - AST created in syn-sem analysis
 */
void codeGenerator(stack_ast_t *ast);

/* gen Assign
 *
 * stack top - variable, expression
 * output - assign expression result to variable
 */
void genAssign(CODE_GEN_PARAMS);

/* genExpr
 *
 * stack top - expression (could be variable, constant, or operation)
 * output - generated expression from ast
 *
 * implicitly convert everything processed (based on operands type and operation type)
 *  - constants statically
 *  - variables dynamically
 *
 * returns number of aux var where the result is stored
 * (could be stored on stack too, might implement later)
 */
int genExpr(CODE_GEN_PARAMS);

/* genFncDeclare
 *
 * stack top - function declaration item
 * output - generated label and get local frame
 * save declared function to ctx
 */
void genFncDeclare(CODE_GEN_PARAMS);

/* genFncCall
 *
 * stack top - function call
 * output - prepare tmp frame, push it to frame stack
 *        - call function
 */
void genFncCall(CODE_GEN_PARAMS);

/* genIf
 *
 * stack top - condition expression
 * output - process condition and generate jump
 *        - use macro PRINT_IF_JMP(JUMP_INSTRUCTION) - see below
 */
void genIf(CODE_GEN_PARAMS);

/* genWhile
 *
 * stack top - condition expression
 * output - proccess condition and generate jump
 *        - PRINT_WHILE_JMP(JUMP_INSTRUCTION) - see below
 */
void genWhile(CODE_GEN_PARAMS);

/* genString
 *
 * stack top - string constant
 * output - generate string in appropriate format
 */
void genString(CODE_GEN_PARAMS);

/* genReturn
 *
 * stack top - return statement
 * output - in main body (ctx->currentFncDeclaration) == NULL
 *          - generate exit 0
 *        - in function declaration
 *          - empty statement - return
 *          - return with expression - generate push of result to stack
 *
 * make sure that returned expression is the same type
 * as current function return value - dynamically
 */
void genReturn(CODE_GEN_PARAMS);

/* genBuiltIns
 *
 * this function is called before code generation
 * generate code for built in function
 * label is the same as function name
 *
 * also generate functions for type conversion and type checks
 */
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

// FUNCTION DECLARE
#define PRINT_RETURN() printf("RETURN\n")

#endif // IFJ_CODE_GEN_H
