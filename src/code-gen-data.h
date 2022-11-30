#ifndef IFJ_CODE_GEN_DATA_H
#define IFJ_CODE_GEN_DATA_H

#include "symtable.h"

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

/* genVarDefs
 *
 * TODO comments
 */
void genVarDefs(ht_table_t *varSymtable, ht_item_t *function);

/* genResolveCondition
 *
 * TODO comments
 */
void genResolveCondition();

/* genExitLabels
 *
 * TODO comments
 */
void genExitLabels();

/* genImplicitConversions
 *
 * TODO comments
 */
void genImplicitConversions();

/* genCond
 *
 * stack - top relational operation
 * output - generated comparison
 *
 * based on operator convert if needed
 *
 * result (boolean) will be stored on stack
 */
void genCond(CODE_GEN_PARAMS);

/* genFncDeclare
 *
 * stack top - function declaration item
 * output - generated label and get local frame
 * save declared function to ctx
 */
void genFncDeclare(CODE_GEN_PARAMS);

#define AUX1 VAR_CODE("GF", "aux%1")
#define AUX2 VAR_CODE("GF", "aux%2")

#endif // IFJ_CODE_GEN_DATA_H
