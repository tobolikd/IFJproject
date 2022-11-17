/* @file ast.h
 *
 * @brief contains data structures and function declarations for AST
 *
 * @author David Tobolik (xtobol06)
 */

#ifndef IFJ_AST_H
#define IFJ_AST_H

#include "symtable.h"

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
    AST_RETURN, // data - AST_type returnType
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

typedef union ast_param_data
{
    ht_item_t *variable;
    int intValue;
    char *stringValue;
    float floatValue;
} AST_param_data;

typedef struct ast_fnc_param
{
    AST_type type; // allowed - AST_INT, AST_FLOAT, AST_string, AST_VAR
    AST_param_data *data;
    struct ast_fnc_param *next;
} AST_fnc_param;

/* function call data
 *
 * functionId - called function
 * params - param_info_t
 *          {
 *              char *varId;
 *              var_type_t type;
 *              struct param_info_t *next;
 *          }
 */
typedef struct
{
    char *functionId;
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
    AST_type returnType;    // AST_RETURN
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
    AST_data *data;
} AST_item;

AST_item *ast_item_const(AST_type type, AST_data *data);

void ast_item_destr(AST_item *deleted);

void function_call_data_destr(AST_function_call_data *data);

#endif // IFJ_AST_H
