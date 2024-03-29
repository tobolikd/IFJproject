/** 
 * IFJ22 compiler
 *
 * @file sem_analyzer.h
 * 
 * @brief functions used in semantic analysis, generated MACROS for sem analysis
 *
 * @authors Adam Gabrys (xgabry01), David Tobolik (xtobol06)
 */

#ifndef IFJ_SEM_ANALYZER_H
#define IFJ_SEM_ANALYZER_H 1

#include "symtable.h"
#include "lex_analyzer.h"
#include "ast.h"

extern ht_table_t *fncTable;

#define CURR_TOKEN_TYPE list->TokenArray[index]->type
#define NEXT_TOKEN index++
#define CURR_FCTION_ID list->TokenArray[index]->data

/* @brief check semantics of the given expression
 * David Tobolik (xtobol06)
 *
 * @param type type of evaluated expression
 * @param start starting token of expression
 * @param end ending token of expression
 *
 * @return true of false according to check result
 */
bool checkFncCall(AST_function_call_data *data);

///@brief inits the symtable and all integrated fctions
///Adam Gabrys (xgabry01)
///@returns hastable with declarations of integrated fctions
ht_table_t *integratedFncSymtable();

///@brief puts all function declarations as items to the hashtable along with their params and returntypes
///Adam Gabrys (xgabry01)
///@param list list with all the tokens
///@param fncSymtable table where we want to put all the items
///@return pointer to hashtable.
///@return NULL when any error occured or if a fction was redeclared
ht_table_t *fncDeclarations(TokenList *list, ht_table_t *fncSymtable);

///@brief inits all integrated fctions and puts all functions from program to hashtable
///Adam Gabrys (xgabry01)
///@usage ht_table_t *table = InitializedHTableFnctionDecs(list);
///@param list List of Tokens
///@return Hashtable with all initialized integrated fctions and all program fctions
///@return NULL when error with redeclaration of fction or other
ht_table_t *fncDeclarationTable(TokenList *list);

///@brief checks if the return type of declared function is okay
///David Tobolik (xtobol06)
///@param list list of Tokens
///@param index for moving within the list
///@param currFncDeclare currently checked fnc declaration
///@return true if return is okay
///@return false if there was an error
bool checkReturn(TokenList *list, int *index, ht_item_t *currFncDeclare);

///@brief checks if function doesnt have identical param IDs in its declaration
///Adam Gabrys (xgabry01)
///@param list list of Tokens
///@param index for moving within the list
///@param currFncDeclare currently checked fnc declaration
///@returns false if there are no same params
///@returns true if there are identical paramIDs
bool paramRedeclaration(TokenList *list,int *index, ht_item_t *currFncDeclare);

#endif // IFJ_SEM_ANALYZER_H
