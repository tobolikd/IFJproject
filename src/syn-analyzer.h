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
#define SYN_ANALYZER_TYPE_N_PARAM_PARAMS TokenList *list, int *index

extern stack_declare_t stackDeclare; // Global variable - stack for Function Frames

typedef struct
{
    ht_table_t *table;     // symtable with variables in main program (body)
    stack_ast_t *stackAST; // stack for building AST
    bool correct;          // true if recursion return true (correct recursive descent), false for error in recursive descent
} SyntaxItem;

SyntaxItem *SyntaxItemCtor(ht_table_t *table, stack_ast_t *stackAST, bool correct);
void SyntaxDtor(SyntaxItem *SyntaxItem);

bool params(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);
bool param(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);
bool typeCheck(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);
bool functionType(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);
bool functionDeclare(TokenList *list, int *index, stack_ast_t *stackSyn);
bool statList(SYN_ANALYZER_PARAMS);
bool statement(SYN_ANALYZER_PARAMS);
bool seqStats(SYN_ANALYZER_PARAMS);
bool checkSyntax(SYN_ANALYZER_PARAMS);
SyntaxItem *synAnalyser(TokenList *list);

#endif // IFJ_SYN_ANALYZER_H
