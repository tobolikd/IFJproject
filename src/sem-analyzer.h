#ifndef IFJ_SEM_ANALYZER_H
#define IFJ_SEM_ANALYZER_H

#include "symtable.h"
#include "lex-analyzer.h"

// type of variable
typedef enum
{
    void_t,
    int_t,
    string_t,
    float_t,
    null_int_t,
    null_string_t,
    null_float_t
} VarType;

/* variable info
 * type - VarType
 * varId - variable name (aka identifier)
 */
typedef struct
{
    VarType type;
    char *varId;
} VarInfo;

/* variable info
 * type - VarType
 * varId - variable name (aka identifier)
 */
typedef struct param_info_t
{
    VarType type;
    char *varId;
    struct param_info_t *next;
} ParamInfo;

/* function info
 * functionId - function name (aka identifier)
 * paramCount - number of parametres
 * params - pointer to list of parametres
 * returnType - VarType of return value
 */
typedef struct
{
    char *functionId;
    int paramCount;
    ParamInfo *params;
    VarType returnType;
} FunctionInfo;

#define FOREACH_EXPR_TYPE(TYPE)\
    TYPE(FunDeclare)\
    TYPE(Assign)\
    TYPE(Operation)\
    TYPE(VarDeclare)\

typedef enum
{
    FOREACH_EXPR_TYPE(GENERATE_ENUM)
} ExprType;

extern const char *EXPR_TYPE_STRING[];

/* @brief check semantics of the given expression
 *
 * @param type type of evaluated expression
 * @param start starting token of expression
 * @param end ending token of expression
 *
 * @return true of false according to check result
 */
bool semCheck(ExprType type, Token *start, Token *end);

#endif // IFJ_SEM_ANALYZER_H
