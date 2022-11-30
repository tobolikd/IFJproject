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

#define SYN_ANALYZER_PARAMS TokenList *list, int *index, ht_table_t *table, stack_ast_t *stackSyn
#define SYN_ANALYZER_TYPE_N_PARAMS_PARAMS TokenList *list, int *index


bool params(SYN_ANALYZER_TYPE_N_PARAMS_PARAMS);
bool param(SYN_ANALYZER_TYPE_N_PARAMS_PARAMS);
bool typeCheck(SYN_ANALYZER_TYPE_N_PARAMS_PARAMS);
bool functionType(SYN_ANALYZER_TYPE_N_PARAMS_PARAMS);
bool functionDeclare(SYN_ANALYZER_PARAMS);
bool statList(SYN_ANALYZER_PARAMS);
bool statement(SYN_ANALYZER_PARAMS);
bool seqStats(SYN_ANALYZER_PARAMS);
bool checkSyntax(SYN_ANALYZER_PARAMS);
bool synAnalyser(TokenList *list);

#endif // IFJ_SYN_ANALYZER_H
