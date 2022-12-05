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
 *      function_declare_data
 *          |- function - pointer to fnc in symtable
 *          |- var_symtable - local symtable for function declaration
 *      int_value - integer constant
 *      stringvalue - string constant
 *      float_value - float constant
 *      function_call_data - structure with fnc call attribs
 *          |- function_id - id of called function - only for debug
 *          |- function - pointer to fnc in symtable
 *          |- params - list of parametres
 *              |- type - data type of parameter
 *              |- data - data of the parameter
 *                  |- one of:
 *                  variable - pointer to var in symtable
 *                  int_value - integer constant
 *                  stringvalue - string constant
 *                  float_value - float constant
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
    AST_ADD = 0,
    AST_SUBTRACT,
    AST_DIVIDE,
    AST_MULTIPLY,

    // string operation
    AST_CONCAT = 10,

    // relation operations
    AST_EQUAL = 20,
    AST_NOT_EQUAL,
    AST_GREATER,
    AST_GREATER_EQUAL,
    AST_LESS,
    AST_LESS_EQUAL,

    // data control
    AST_ASSIGN = 30,
    AST_VAR,    // data - ht_item_t *variable

    // constants
    AST_INT = 40,    // data - int int_value
    AST_STRING, // data - char *string_value
    AST_FLOAT,  // data - double float_value
    AST_NULL,

    // fnc declaration
    AST_FUNCTION_DECLARE = 50,   // data - ht_item_t *function

    // program control (jumps)
    AST_FUNCTION_CALL = 60,  // data - AST_function_call_data *function_call_data
    AST_RETURN_VOID,
    AST_RETURN_EXPR,
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
    AST_END_EXPRESSION=98,
    AST_END_BLOCK = 99
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
    int int_value;
    char *string_value;
    double float_value;
} AST_param_data;

typedef struct ast_fnc_param
{
    AST_param_type type; // allowed - AST_P_INT, AST_P_FLOAT, AST_P_STRING, AST_P_VAR, AST_P_NULL
    AST_param_data *data;
    struct ast_fnc_param *next;
} AST_fnc_param;

/* function call data
 *
 * function_id - function name - only for debug info
 *              pointer is NOT valid after destructing
 * function - pointer to symtable
 * params - list of parametres
 */
typedef struct
{
    char *function_id;
    ht_item_t *function;
    AST_fnc_param *params;
} AST_function_call_data;

/* function declare data
 *
 * function - pointer to symtable
 * var_symtable - local symtable for function declare
 */
typedef struct
{
    ht_item_t *function;
    ht_table_t *var_symtable;
} AST_function_declare_data;

/* AST_item->data for AST_TYPES
 *
 * not mentioned types have AST_item->data = NULL
 */
typedef union
{
    ht_item_t *variable;    // for AST_VAR
    int int_value;           // AST_INT
    char *string_value;      // AST_STRING
    double float_value;       // AST_FLOAT
    AST_function_call_data *function_call_data;   // AST_FUNCTION_CALL
    AST_function_declare_data *function_declare_data; //AST_FUNCTION_DECLARE
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
AST_item *ast_item_const(AST_type type, void *data);

/* ast_item_destr
 *  - frees AST item with all data EXCEPT for the symtable data
 *  note: symtable data will be freed on symtable destruction
 */
void ast_item_destr(AST_item *deleted);

/* fnc_declare_data_const
 *  - allocate fnc declare data and fill with data
 *  function - pointer to declared function in symtable
 *  var_symtable - local variable symtable for declared function
 */
AST_function_declare_data *fnc_declare_data_const(ht_item_t *function, ht_table_t *var_symtable);

/* fnc_call_data_const
 *  - allocate and initialize function call data structure
 *  fnc_symtable - pointer to
 *  function - pointer to symtable
 */
AST_function_call_data *fnc_call_data_const(ht_table_t *fnc_symtable, char *function_id);

/* fnc_call_data_add_param
 *  - adds parameter to function call data acording to param type
 *  type - type of parameter {AST_P_NULL, AST_P_INT, AST_P_STRING, AST_P_FLOAT, AST_P_VAR}
 *  param - pointer to the parameter {NULL, int *, char *, double *, ht_item_t *}
 */
void fnc_call_data_add_param(AST_function_call_data *data, AST_param_type type, void *param);

/* function_call_data_destr
 *  - frees function call data EXCEPT for the symtable data
 *  note: symtable data will be freed on symtable destruction
 */
void fnc_call_data_destr(AST_function_call_data *data);

#endif // IFJ_AST_H
