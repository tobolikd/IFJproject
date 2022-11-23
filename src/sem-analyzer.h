#ifndef IFJ_SEM_ANALYZER_H
#define IFJ_SEM_ANALYZER_H

#include "symtable.h"
#include "lex-analyzer.h"
#include "ast.h"

/* @brief check semantics of the given expression
 *
 * @param type type of evaluated expression
 * @param start starting token of expression
 * @param end ending token of expression
 *
 * @return true of false according to check result
 */
bool checkFncCall(AST_function_call_data *data);

#endif // IFJ_SEM_ANALYZER_H
