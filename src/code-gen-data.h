#ifndef IFJ_CODE_GEN_DATA_H
#define IFJ_CODE_GEN_DATA_H

typedef enum {
    BLOCK_IF,
    BLOCK_ELSE,
    BLOCK_WHILE,
    BLOCK_DECLARE
} code_block_type;

typedef struct {
    code_block_type type;
    int labelNum;
} code_block;

/* genBuiltIns
 *
 * this function is called before code generation
 * generate code for built in function
 * label is the same as function name
 *
 * also generate functions for type conversion and type checks
 */
void genBuiltIns();

/* genResolveCondition
 *
 * TODO comments
 */
void genResolveCondition();

#endif // IFJ_CODE_GEN_DATA_H
