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

/* genStatic
 * David Tobolik (xtobol06)
 *
 * this function is called before code generation
 * generate code for built in function
 * label is the same as function name
 *
 * also generate functions for type conversion and type checks
 */
void genStatic();

/* genVarDefinitions
 * David Tobolik (xtobol06)
 *
 * generates variable definitions in IFJcode22 from symtable
 * doesnt generate function parametres (if given)
 */
void genVarDefinitions(ht_table_t *varSymtable, ht_item_t *function);

/* genResolveCondition
 * David Tobolik (xtobol06)
 *
 * generates aux function for condition resolve
 * gets 1 operand on stack (any type)
 * returns bool value on stack, which is result of condition
 * false - "", "0", null, 0, 0.0, false
 * true - anything else
 */
void genResolveCondition();

/* genExitLabels
 * David Tobolik (xtobol06)
 *
 * generates exit labels for runtime errors
 */
void genExitLabels();

/* genImplicitConversions
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
void genImplicitConversions();

/* genMathEqual
 * Gabriel Biel (xbielg00)
 *
 * Generates math function equal called with label: math%equal
 * Expects two operands on top of the stack.
 * Result is a boolean value on top of the stack. With operands poped.
 */
void genMathEqual();

/* genSemanticTypeCheck
 * Gabriel Biel (xbielg00)
 *
 * Generates comparison operands type. Expects operand on top of the stack. 
 * Possible label call:
 * {type%check%int},{type%check%float},{type%check%string},{type%check%nil}
 * {type%check%int%nil},{type%check%float%nil},{type%check%string%nil}
 * 
 * Returns with the operand on top or with thrown error, when types are different.
 * 
 */
void genSemanticTypeCheck();

/* genBuiltInFcs
 * Adam Gabrys (xgabry01)
 *
 * generates all built-functions
 *
 * readi, readf, reads, chr, ord, strlen, substr
 */
void genBuiltInFcs();

#define AUX1 VAR_CODE("GF", "aux%1")
#define AUX2 VAR_CODE("GF", "aux%2")

#endif // IFJ_CODE_GEN_DATA_H
