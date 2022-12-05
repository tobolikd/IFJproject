/* @file code_gen_static.h
 *
 * @brief data structures for code gen and functions generating static parts of program
 *
 * @author David Tobolik (xtobol06)
 */

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
    int label_num;
} code_block;

/* gen_static
 * David Tobolik (xtobol06)
 *
 * this function is called before code generation
 * generate code for built in function
 * label is the same as function name
 *
 * also generate functions for type conversion and type checks
 */
void gen_static();

/* gen_var_definitions
 * David Tobolik (xtobol06)
 *
 * generates variable definitions in IFJcode22 from symtable
 * doesnt generate function parametres (if given)
 */
void gen_var_definitions(ht_table_t *var_symtable, ht_item_t *function);

/* gen_resolve_condition
 * David Tobolik (xtobol06)
 *
 * generates aux function for condition resolve
 * gets 1 operand on stack (any type)
 * returns bool value on stack, which is result of condition
 * false - "", "0", null, 0, 0.0, false
 * true - anything else
 */
void gen_resolve_condition();

/* gen_exit_labels
 * David Tobolik (xtobol06)
 *
 * generates exit labels for runtime errors
 */
void gen_exit_labels();

/* gen_implicit_conversions
 * David Tobolik (xtobol06)
 *
 * generates implicit conversions for arithmetic, string and relational operations
 * 
 * arithmetic
 * {int, int} {float, float} - same
 * {int, float} - float
 * {nil, {int, nil}} - int
 * {nil, float} - float
 * others - error
 * 
 * string
 * {string, string} - same
 * {string, nil} - string
 * others - error
 * 
 * relational
 * {string, {int, float}} - error
 * others - according to arithmetic or string logic
 */
void gen_implicit_conversions();

void gen_data_type_comparisons();

void gen_semantic_type_check();

void gen_built_in_fcs();

#define AUX1 VAR_CODE("GF", "aux%1")
#define AUX2 VAR_CODE("GF", "aux%2")

#endif // IFJ_CODE_GEN_DATA_H
