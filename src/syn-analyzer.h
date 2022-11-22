/**
 * @file syn-analyzer.h
 * @author Jakub Mikysek (xmikys03)
 * @brief Syntatic Analyser for interpreter IFJcode22 - Header File
 */

#ifndef IFJ_SYN_ANALYZER_H
#define IFJ_SYN_ANALYZER_H

#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "error-codes.h"
#include "symtable.h"

bool callParams(TokenList *list, int *index);
bool callParam(TokenList *list, int *index);
bool params(TokenList *list, int *index);
bool param(TokenList *list, int *index);
bool typeCheck(TokenList *list, int *index);
bool functionType(TokenList *list, int *index);
bool functionDeclare(TokenList *list, int *index);
bool statList(TokenList *list, int *index);
bool statement(TokenList *list, int *index);
bool seqStats(TokenList *list, int *index);
bool checkSyntax(TokenList *list, int *index);
bool synAnalyser(TokenList *list);

#endif // IFJ_SYN_ANALYZER_H
