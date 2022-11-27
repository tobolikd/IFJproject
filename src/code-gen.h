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
void genString(char *string);

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

#define SPACE printf(" ");

// macros for instructions
#define INST_MOVE(var, symb) do { printf("MOVE "); var; SPACE symb; printf("\n"); } while (0)
#define INST_CREATEFRAME() printf("PUSHFRAME\n")
#define INST_PUSHFRAME() printf("PUSHFRAME\n")
#define INST_POPFRAME() printf("POPFRAME\n")
#define INST_DEFVAR(var) do { printf("DEFVAR "); var; printf("\n"); } while (0)
#define INST_CALL(label) do { printf("CALL "); label; printf("\n"); } while (0)
#define INST_RETURN() printf("RETURN\n")
#define INST_PUSHS(symb) do { printf("PUSHS "); symb; printf("\n"); } while (0)
#define INST_POPS(var) do { printf("POPS "); var; printf("\n"); } while (0)
#define INST_CLEARS() printf("CLEARS\n")
#define INST_ADD(var, symb1, symb2) do { printf("ADD "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_SUB(var, symb1, symb2) do { printf("SUB "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_MUL(var, symb1, symb2) do { printf("MUL "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_DIV(var, symb1, symb2) do { printf("DIV "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_IDIV(var, symb1, symb2) do { printf("IDIV "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_ADDS() printf("ADDS\n")
#define INST_SUBS() printf("SUBS\n")
#define INST_MULS() printf("MULS\n")
#define INST_DIVS() printf("DIVS\n")
#define INST_LT(var, symb1, symb2) do { printf("LT "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_GT(var, symb1, symb2) do { printf("GT "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_EQ(var, symb1, symb2) do { printf("EQ "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_LTS() printf("LTS\n")
#define INST_GTS() printf("GTS\n")
#define INST_EQS() printf("EQS\n")
#define INST_AND(var, symb1, symb2) do { printf("AND "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_OR(var, symb1, symb2) do { printf("OR "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_NOT(var, symb) do { printf("NOT "); var; SPACE symb; printf("\n"); } while (0)
#define INST_ANDS() printf("ANDS\n")
#define INST_ORS() printf("ORS\n")
#define INST_NOTS() printf("NOTS\n")
#define INST_INT2FLOAT(var, symb) do { printf("INT2FLOAT "); var; SPACE symb; printf("\n"); } while (0)
#define INST_FLOAT2INT(var, symb) do { printf("FLOAT2INT "); var; SPACE symb; printf("\n"); } while (0)
#define INST_INT2CHAR(var, symb) do { printf("INT2CHAR"); var; SPACE symb; printf("\n"); } while (0)
#define INST_STRI2INT(var, symb) do { printf("STRI2INT"); var; SPACE symb; printf("\n"); } while (0)
#define INST_INT2FLOATS() printf("INT2FLOATS\n")
#define INST_FLOAT2INTS() printf("FLOAT2INTS\n")
#define INST_INT2CHARS() printf("INT2CHARS\n")
#define INST_STRI2INTS() printf("STRI2INTS\n")
#define INST_READ(var, type) do { printf("READ "); var; SPACE printf(type); printf("\n"); } while (0)
#define INST_WRITE(symb) do { printf("WRITE "); symb; printf("\n"); } while (0)
#define INST_CONCAT(var, symb1, symb2) do { printf("CONCAT "); var; SPACE, symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_STRLEN(var, symb) do { printf("STRLEN "); var; SPACE symb; printf("\n"); } while (0)
#define INST_GETCHAR(var, symb1, symb2) do { printf("GETACHAR "); var; SPACE, symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_SETCHAR(var, symb1, symb2) do { printf("SETCHAR "); var; SPACE, symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_TYPE(var, symb) do { printf("TYPE "); var; SPACE symb printf("\n"); } while (0)
#define INST_LABEL(label) do { printf("LABEL "); label; printf("\n"); } while (0)
#define INST_JUMP(label) do { printf("JUMP "); label; printf("\n"); } while (0)
#define INST_JUMPIFEQ(label, symb1, symb2) do { printf("JUMPIFEQ "); label; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_JUMPIFNEQ(label, symb1, symb2) do { printf("JUMPIFNEQ "); label; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_JUMPIFEQS(label) do { printf("JUMPIFEQS "); label; printf("\n") } while (0)
#define INST_JUMPIFNEQS(label) do { printf("JUMPIFNEQS "); label; printf("\n"); } while (0)
#define INST_EXIT(value) printf("EXIT %d\n", value)
#define INST_BREAK() printf("BREAK\n")
#define INST_DPRINT() printf("DPRINT\n")

// generating symbols
#define VAR_AUX(num) printf("<aux%%%d>", num)
#define VAR_CODE(id) printf("<%s>", id)
#define CONST_FLOAT(value) printf("<%a>", value)
#define CONST_INT(value) printf("<%d>", value)
#define CONST_STRING(ptr) do { printf("<"); genString(ptr); printf(">"); } while

#define LABEL(label) printf("<%s>", label)

// generating condition labels

// if else
#define LABEL_ELSE() printf("<else%%%d>", stack_code_block_top(&ctx->blockStack)->labelNum)
#define LABEL_ENDELSE() printf("<end_else%%%d>", stack_code_block_top(&ctx->blockStack)->labelNum)

// while
#define LABEL_WHILE_BEGIN() printf("<while_begin%%%d>", stack_code_block_top(&ctx->blockStack)->labelNum)
#define LABEL_WHILE_END() printf("<while_end%%%d>", stack_code_block_top(&ctx->blockStack)->labelNum)

#endif // IFJ_CODE_GEN_H
