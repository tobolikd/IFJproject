#ifndef IFJ_SEM_ANALYZER_H
#define IFJ_SEM_ANALYZER_H

#include "symtable.h"
#include "lex-analyzer.h"
#include "ast.h"

extern ht_table_t *fncTable;

/* @brief check semantics of the given expression
 *
 * @param type type of evaluated expression
 * @param start starting token of expression
 * @param end ending token of expression
 *
 * @return true of false according to check result
 */
bool checkFncCall(AST_function_call_data *data);

///@brief inits the symtable and all integrated fctions
///@returns hastable
ht_table_t *initFncSymtable();

///@brief puts all function declarations as items to the hashtable along with their params and returntypes
///@param list list with all the tokens
///@param fncSymtable table where we want to put all the items
///@return pointer to hashtable.
///@return NULL when any error occured or if a fction was redeclared
ht_table_t *PutFncsDecToHT(TokenList *list, ht_table_t *fncSymtable);

///@brief inits all integrated fctions and puts all functions from program to hashtable
///@usage ht_table_t *table = InitializedHTableFnctionDecs(list);
///@param list List of Tokens
///@return Hashtable with all initialized integrated fctions and all program fctions
///@return NULL when error with redeclaration of fction or other
ht_table_t *InitializedHTableFnctionDecs(TokenList *list);

#endif // IFJ_SEM_ANALYZER_H
