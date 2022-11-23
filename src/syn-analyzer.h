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

/**
 * @brief GRAMMAR: <call-params> -> , <var> <call-params> || , <literal> <call-params> || eps
 * callParam function calls this function for parameter check with multiple params
 * 
 * @param list List of Tokens
 * @param index current position in Array of Tokens
 * @return true if is syntax is okay
 * @return false if SYNTAX_ERR appeared
 */
bool callParams(TokenList *list, int *index);

/**
 * @brief GRAMMAR: <call-param> -> <var> <call-params> || <literal> <call-params> || eps
 * Use this function for parameter check in function call, for more than one param fuction calls another function callParams
 * 
 * @param list List of Tokens
 * @param index current position in Array of Tokens
 * @return true if is syntax is okay
 * @return false if SYNTAX_ERR appeared
 */
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
