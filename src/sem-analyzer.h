#ifndef IFJ_SEM_ANALYZER_H
#define IFJ_SEM_ANALYZER_H

#include "symtable.h"
#include "lex-analyzer.h"

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
bool semCheck(ExprType type, TokenList *tokenArray, int startToken, int endToken);

#endif // IFJ_SEM_ANALYZER_H
