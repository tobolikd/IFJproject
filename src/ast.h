/* @file ast.h
 *
 * @brief contains data structures and function declarations for AST
 *
 * @author David Tobolik (xtobol06)
 */

#ifndef IFJ_AST_H
#define IFJ_AST_H

#include "symtable.h"

/* Overview of structures
 *
 * AST_item
 *  |- type - see AST_type
 *  |- data
 *      |- one of:
 *      variable - pointer to var in symtable
 *      function - pointer to fnc in symtable
 *      intValue - integer constant
 *      stringvalue - string constant
 *      floatValue - float constant
 *      blank - if the return statement returns value or not
 *      functionCallData - structure with fnc call attribs
 *          |- function - pointer to fnc in symtable
 *          |- params - list of parametres
 *              |- type - data type of parameter
 *              |- data - data of the parameter
 *                  |- one of:
 *                  variable - pointer to var in symtable
 *                  intValue - integer constant
 *                  stringvalue - string constant
 *                  floatValue - float constant
 *                  NULL - in case of AST_P_NULL
 *              |- next - pointer to next parameter
 */

/* AST_type - type of AST item
 *
 * some types are connected with data (see comments next to them)
 *
 * types with no comment, have no data
 */
typedef enum
{
    // arithmetic operations
    AST_ADD,
    AST_SUBTRACT,
    AST_DIVIDE,
    AST_MULTIPLY,

    // string operation
    AST_CONCAT,

    // relation operations
    AST_EQUAL,
    AST_NOT_EQUAL,
    AST_GREATER,
    AST_GREATER_EQUAL,
    AST_LESS,
    AST_LESS_EQUAL,

    // data control
    AST_ASSIGN,
    AST_VAR,    // data - ht_item_t *variable

    // constants
    AST_INT,    // data - int intValue
    AST_STRING, // data - char *stringValue
    AST_FLOAT,  // data - float floatValue
    AST_NULL,

    // fnc declaration
    AST_FUNCTION_DECLARE,   // data - ht_item_t *function

    // program control (jumps)
    AST_FUNCTION_CALL,  // data - AST_function_call_data *functionCallData
    AST_RETURN, // data - bool blank
    AST_IF,
    AST_ELSE,
    AST_WHILE,

    /* end - (maybe semicolon) or closing bracket, depending on the expression type
     *
     * these types have to be ended with AST_END_BLOCK
     * AST_FUNCTION_DECLARE
     * AST_IF
     * AST_ELSE
     * AST_WHILE
     */
    AST_END_BLOCK
} AST_type;

typedef enum
{
    AST_P_INT,
    AST_P_STRING,
    AST_P_FLOAT,
    AST_P_NULL,
    AST_P_VAR
} AST_param_type;

typedef union ast_param_data
{
    ht_item_t *variable;
    int intValue;
    char *stringValue;
    float floatValue;
} AST_param_data;

typedef struct ast_fnc_param
{
    AST_param_type type; // allowed - AST_P_INT, AST_P_FLOAT, AST_P_STRING, AST_P_VAR, AST_P_NULL
    AST_param_data *data;
    struct ast_fnc_param *next;
} AST_fnc_param;

/* function call data
 *
 * function - pointer to symtable
 * params - list of parametres
 */
typedef struct
{
    ht_item_t *function;
    AST_fnc_param *params;
} AST_function_call_data;

/* AST_item->data for AST_TYPES
 *
 * not mentioned types have AST_item->data = NULL
 */
typedef union
{
    ht_item_t *variable;    // for AST_VAR
    int intValue;           // AST_INT
    char *stringValue;      // AST_STRING
    float floatValue;       // AST_FLOAT
    ht_item_t *function;    // AST_DECLARE
    AST_function_call_data *functionCallData;   // AST_FUNCTION_CALL
    bool blank;             // AST_RETURN
                            // whether the return statement is empty or not
                            // needed to determine if another item is part of
                            // the return call
} AST_data;

/* AST_item - items in the AST
 *
 * type - AST_type of the construction
 * data - if assigned to the AST_type contains pointer
 *        to AST_data of responding type
 */
typedef struct
{
    AST_type type;
    AST_data *data; // NULL or some data acording to type
                    // see AST_data
} AST_item;

/* ast_item_const
 *  - item constructor
 *  - returns allocated item with type and data
 */
AST_item *ast_item_const(AST_type type, AST_data *data);

/* ast_item_destr
 *  - frees AST item with all data EXCEPT for the symtable data
 *  note: symtable data will be freed on symtable destruction
 */
void ast_item_destr(AST_item *deleted);

/* fnc_call_data_init
 *  - initialize function call data structure
 *  data - pointer to data to be initialized
 *  function - pointer to symtable
 */
void fnc_call_data_init(AST_function_call_data *data, ht_item_t *function);

/* fnc_call_data_add_param
 *  - adds parameter to function call data acording to param type
 *  data - pointer to data where param will be added
 *  type - type of parameter {AST_P_NULL, AST_P_INT, AST_P_STRING, AST_P_FLOAT, AST_P_VAR}
 *  param - pointer to the parameter {NULL, int *, char *, float *, ht_item_t *}
 */
void fnc_call_data_add_param(AST_function_call_data *data, AST_param_type type, void *param);

/* function_call_data_destr
 *  - frees function call data EXCEPT for the symtable data
 *  note: symtable data will be freed on symtable destruction
 */
void function_call_data_destr(AST_function_call_data *data);

#endif // IFJ_AST_H
