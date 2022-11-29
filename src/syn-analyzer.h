/**
 * @file syn-analyzer.h
 * @author Jakub Mikysek (xmikys03)
 * @brief Syntatic Analyser for interpreter IFJcode22 - Header File
 */

#ifndef IFJ_SYN_ANALYZER_H
#define IFJ_SYN_ANALYZER_H 1 

#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "error-codes.h"
#include "symtable.h"


bool params(TokenList *list, int *index, ht_item_t *curFunction);
bool param(TokenList *list, int *index, ht_item_t *curFunction);
var_type_t getType(TokenList *list, int *index);
bool typeCheck(TokenList *list, int *index);
bool functionType(TokenList *list, int *index);
bool functionDeclare(TokenList *list, int *index, ht_table_t *table);
bool statList(TokenList *list, int *index, ht_table_t *table);
bool statement(TokenList *list, int *index, ht_table_t *table);
bool seqStats(TokenList *list, int *index, ht_table_t *table);
bool checkSyntax(TokenList *list, int *index, ht_table_t *table);
bool synAnalyser(TokenList *list);

#endif // IFJ_SYN_ANALYZER_H
