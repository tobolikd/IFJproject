#ifndef IFJ_CODE_GEN_H
#define IFJ_CODE_GEN_H 1

#include "ast.h"
#include "stack.h"
#include "code_gen_static.h"
#include "symtable.h"
#include "error-codes.h"

typedef struct {
    // counts are used to generate unique labels and auxiliary variable names
    unsigned if_count;
    unsigned else_number;
    unsigned while_count;
    ht_item_t *current_fnc_declaration; // pointer to fnc symtable
                                      // NULL - in main body
                                      // ptr - in function declaration
    stack_code_block_t block_stack;  // stack of code blocks for nested blocks
} codeGenCtx;

#define CODE_GEN_PARAMS stack_ast_t *ast, codeGenCtx *ctx

/* code_generator
 *
 * ast - AST created in syn-sem analysis
 */
void code_generator(stack_ast_t *ast, ht_table_t *varSymtable);

/* gen Assign
 *
 * stack top - variable, expression
 * output - assign expression result to variable
 */
void gen_assign(stack_ast_t *ast);

/* gen_expr
 *
 * stack top - expression (could be variable, constant, or operation)
 * output - generated expression from ast
 *
 * implicitly convert everything processed (based on operands type and operation type)
 *  - constants statically
 *  - variables dynamically
 *
 * result will be stored on stack
 */
void gen_expr(stack_ast_t *ast);

/* gen_fnc_call
 *
 * stack top - function call
 * output - prepare tmp frame, push it to frame stack
 *        - call function
 */
void gen_fnc_call(stack_ast_t *ast);

/* gen_write
 *
 * stack top - write function call
 * output - write every term
 */
void gen_write(stack_ast_t *ast);

/* gen_condition
 *
 * stack top - condition expression
 * output - process and resolve condition
 *        - jump acording to current block type (if, while)
 */
void gen_condition(CODE_GEN_PARAMS);

/* gen_string
 *
 * stack top - string constant
 * output - generate string in appropriate format
 * note: just print, dont pop
 */
void gen_string(char *str);

/* gen_return
 *
 * stack top - return statement
 * output - in main body (ctx->current_fnc_declaration) == NULL
 *          - generate exit 0
 *        - in function declaration
 *          - empty statement - return
 *          - return with expression - generate push of result to stack
 *
 * make sure that returned expression is the same type
 * as current function return value - dynamically
 */
void gen_return(CODE_GEN_PARAMS);


#define SPACE printf(" ");

// macros for instructions
#define INST_MOVE(var, symb) do { printf("MOVE "); var; SPACE symb; printf("\n"); } while (0)
#define INST_CREATEFRAME() printf("CREATEFRAME\n")
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
#define INST_INT2CHAR(var, symb) do { printf("INT2CHAR "); var; SPACE symb; printf("\n"); } while (0)
#define INST_STRI2INT(var, symb1,symb2) do { printf("STRI2INT "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_INT2FLOATS() printf("INT2FLOATS\n")
#define INST_FLOAT2INTS() printf("FLOAT2INTS\n")
#define INST_INT2CHARS() printf("INT2CHARS\n")
#define INST_STRI2INTS() printf("STRI2INTS\n")
#define INST_READ(var, type) do { printf("READ "); var; SPACE printf(type); printf("\n"); } while (0)
#define INST_WRITE(symb) do { printf("WRITE "); symb; printf("\n"); } while (0)
#define INST_CONCAT(var, symb1, symb2) do { printf("CONCAT "); var; SPACE; symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_STRLEN(var, symb) do { printf("STRLEN "); var; SPACE symb; printf("\n"); } while (0)
#define INST_GETCHAR(var, symb1, symb2) do { printf("GETCHAR "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_SETCHAR(var, symb1, symb2) do { printf("SETCHAR "); var; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_TYPE(var, symb) do { printf("TYPE "); var; SPACE symb; printf("\n"); } while (0)
#define INST_LABEL(label) do { printf("LABEL "); label; printf("\n"); } while (0)
#define INST_JUMP(label) do { printf("JUMP "); label; printf("\n"); } while (0)
#define INST_JUMPIFEQ(label, symb1, symb2) do { printf("JUMPIFEQ "); label; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_JUMPIFNEQ(label, symb1, symb2) do { printf("JUMPIFNEQ "); label; SPACE symb1; SPACE symb2; printf("\n"); } while (0)
#define INST_JUMPIFEQS(label) do { printf("JUMPIFEQS "); label; printf("\n"); } while (0)
#define INST_JUMPIFNEQS(label) do { printf("JUMPIFNEQS "); label; printf("\n"); } while (0)
#define INST_EXIT(value) do { printf("EXIT "); value; printf("\n"); } while (0)
#define INST_BREAK() printf("BREAK\n")
#define INST_DPRINT(symb) do { printf("DPRINT "); symb; printf("\n"); } while (0)

// generating symbols
#define VAR_BLACKHOLE() printf("GF@black%%hole")
#define VAR_AUX(num) printf("LF@aux%%%d", num)
#define VAR_CODE(frame, id) printf("%s@%s", frame, id)
#define CONST_FLOAT(value) printf("float@%a", value)
#define CONST_INT(value) printf("int@%d", value)
#define CONST_BOOL(value) printf("bool@%s", value)
#define CONST_NIL() printf("nil@nil")
#define CONST_STRING(ptr) gen_string(ptr)

#define LABEL(label) printf("%s", label)
// checking initialization
#define CHECK_INIT(var) do{                                                     \
    INST_TYPE(VAR_BLACKHOLE(), var);                                            \
    INST_JUMPIFEQ(LABEL("not%init"), VAR_BLACKHOLE(), CONST_STRING(""));        \
} while (0)

// generating condition labels

// if else
#define LABEL_ELSE() printf("else%%%d", stack_code_block_top(&ctx->block_stack)->labelNum)
#define LABEL_ENDELSE() printf("end_else%%%d", stack_code_block_top(&ctx->block_stack)->labelNum)

// while
#define LABEL_WHILE_BEGIN() printf("while_begin%%%d", stack_code_block_top(&ctx->block_stack)->labelNum)
#define LABEL_WHILE_END() printf("while_end%%%d", stack_code_block_top(&ctx->block_stack)->labelNum)

// function declare
#define LABEL_FNC_DECLARE_END() printf("end%%fnc%%%s", ctx->current_fnc_declaration->identifier)

// help functions
#define AST_POP() stack_ast_pop(ast)
#define AST_TOP() stack_ast_top(ast)

#if DEBUG == 1
    #define COMMENT(...) do { printf("\n\n# "); printf(__VA_ARGS__); printf("\n"); } while (0)
#else
    #define COMMENT(...)
#endif


#endif // IFJ_CODE_GEN_H
