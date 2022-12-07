/**
 * IFJ22 compiler
 *
 * @file ast.h 
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
 *      functionDeclareData
 *          |- function - pointer to fnc in symtable
 *          |- varSymtable - local symtable for function declaration
 *      intValue - integer constant
 *      stringvalue - string constant
 *      floatValue - float constant
 *      functionCallData - structure with fnc call attribs
 *          |- functionId - id of called function - only for debug
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
    AST_INT = 40,    // data - int intValue
    AST_STRING, // data - char *stringValue
    AST_FLOAT,  // data - double floatValue
    AST_NULL,

    // fnc declaration
    AST_FUNCTION_DECLARE = 50,   // data - ht_item_t *function

    // program control (jumps)
    AST_FUNCTION_CALL = 60,  // data - AST_function_call_data *functionCallData
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
    ht_item_t *variable;// AST_P_VAR
    int intValue;      // AST_P_INT
    char *stringValue; // AST_P_STRING
    double floatValue; // AST_P_FLOAT
} AST_param_data;

/* AST_fnc_param
 * 
 * type - parameter type
 * data - AST_P_NULL has NULL data, others according to type
 */
typedef struct ast_fnc_param
{
    AST_param_type type; // AST_P_INT, AST_P_FLOAT, AST_P_STRING, AST_P_VAR, AST_P_NULL
    AST_param_data *data;
    struct ast_fnc_param *next;
} AST_fnc_param;

/* function call data
 *
 * functionId - function name - only for debug info
 *              pointer is NOT valid after destructing
 * function - pointer to symtable
 * params - list of parametres
 */
typedef struct
{
    char *functionId;
    ht_item_t *function;
    AST_fnc_param *params; // list of parametres
} AST_function_call_data;

/* function declare data
 *
 * function - pointer to symtable
 * varSymtable - local symtable for function declare
 */
typedef struct
{
    ht_item_t *function;
    ht_table_t *varSymtable;
} AST_function_declare_data;

/* AST_item->data for AST_TYPES
 *
 * not mentioned types have AST_item->data = NULL
 */
typedef union
{
    ht_item_t *variable;    // for AST_VAR
    int intValue;           // AST_INT
    char *stringValue;      // AST_STRING
    double floatValue;       // AST_FLOAT
    AST_function_call_data *functionCallData;   // AST_FUNCTION_CALL
    AST_function_declare_data *functionDeclareData; //AST_FUNCTION_DECLARE
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

/* astItemConst
 *  - item constructor
 *  - returns allocated item with type and data
 */
AST_item *astItemConst(AST_type type, void *data);

/* astItemDestr
 *  - frees AST item with all data EXCEPT for the symtable data
 *  note: symtable data will be freed on symtable destruction
 */
void astItemDestr(AST_item *deleted);

/* fncDeclareDataConst
 *  - allocate fnc declare data and fill with data
 *  function - pointer to declared function in symtable
 *  varSymtable - local variable symtable for declared function
 */
AST_function_declare_data *fncDeclareDataConst(ht_item_t *function, ht_table_t *varSymtable);

/* fncCallDataConst
 *  - allocate and initialize function call data structure
 *  fncSymtable - pointer to
 *  function - pointer to symtable
 */
AST_function_call_data *fncCallDataConst(ht_table_t *fncSymtable, char *functionId);

/* fncCallDataAddParam
 *  - adds parameter to function call data acording to param type
 *  type - type of parameter {AST_P_NULL, AST_P_INT, AST_P_STRING, AST_P_FLOAT, AST_P_VAR}
 *  param - pointer to the parameter {NULL, int *, char *, double *, ht_item_t *}
 */
void fncCallDataAddParam(AST_function_call_data *data, AST_param_type type, void *param);

/* function_call_data_destr
 *  - frees function call data EXCEPT for the symtable data
 *  note: symtable data will be freed on symtable destruction
 */
void fncCallDataDestr(AST_function_call_data *data);

#endif // IFJ_AST_H
