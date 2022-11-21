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

void params(TokenList *list, int *index);
void param(TokenList *list, int *index);
void type(TokenList *list, int *index);
void functionType(TokenList *list, int *index);
void statList(TokenList *list, int *index);
void functionDeclare(TokenList *list, int *index);
void statement(TokenList *list, int *index);
void seqStats(TokenList *list, int *index);
void checkSyntax(TokenList *list, int *index);
void synAnalyser(TokenList *list);

#endif // IFJ_SYN_ANALYZER_H
