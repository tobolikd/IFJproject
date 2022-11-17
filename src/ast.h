/* ast.h
 *
 * @brief contains data structures and functions for AST 
 *
 * @author David Tobolik (xtobol06)
 */

#ifdef IFJ_AST_H
#define IFJ_AST_H

typedef enum
{
    // arithmetic operations
    AST_ADD,
    AST_SUBTRACT,
    AST_DIVIDE,
    AST_MULTIPLY,
    
    // string operation
    AST_CONCAT,

    // raw data
    AST_INT,
    AST_STRING,
    AST_FLOAT,

    AST_,
    AST_,
} AST_type;

typedef union
{
    char *id;
} AST_data;

typedef struct
{
    AST_type type;
    bool binaryOp;
    AST_data;
} AST_item;

#endif // IFJ_AST_H
