/**
 * @file syn-analyzer.h
 * @author Jakub Mikysek (xmikys03)
 * @brief Syntatic Analyser for interpreter IFJcode22 - Header File
 */

#ifndef IFJ_SYN_ANALYZER_H
#define IFJ_SYN_ANALYZER_H 1

#include "lex_analyzer.h"
#include "symtable.h"
#include "stack.h"

#define SYN_ANALYZER_PARAMS TokenList *list, int *index, ht_table_t *table, stack_ast_t *stackSyn
#define SYN_ANALYZER_TYPE_N_PARAM_PARAMS TokenList *list, int *index

#define TOKEN_TYPE list->TokenArray[*index]->type
#define TOKEN_LINE_NUM list->TokenArray[*index]->lineNum

#define CHECK_END_PROGRAM() \
    do { if ((*index) == list->length || list->TokenArray[*index]->type == t_EOF){return true;} \
         if (programContinue == false){return false;} } while(0)

extern stack_declare_t stackDeclare; // Global variable - stack for Function Frames

typedef struct
{
    ht_table_t *table;     // symtable with variables in main program (body)
    stack_ast_t *stackAST; // stack for building AST
    bool correct;          // true if recursion return true (correct recursive descent), false for error in recursive descent
} SyntaxItem;

/**
 * @brief create Syntax Item
 * 
 * @param table symtable with defitions of variables
 * @param stackAST stack with AST
 * @param correct true/false if recursive descent was without error or not
 * @return SyntaxItem*
 */
SyntaxItem *syntaxItemCtor(ht_table_t *table, stack_ast_t *stackAST, bool correct);

/**
 * @brief destroy Syntax Item
 * 
 * @param SyntaxItem 
 */
void syntaxDtor(SyntaxItem *SyntaxItem);

// <params> -> , <type> <var> <params> || eps
bool params(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);
// <param> -> <type> <var> <params> || eps
bool param(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);

/**
 * @brief <type> -> int || string || float
 * @param list list of Tokens
 * @param index index in array of Tokens
 * @return false if Syntax Error appeared, true if success
 */
bool checkType(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);

/**
 * @brief <type> -> int || string || float || ?int || ?string || ?float
 * @param list list of Tokens
 * @param index index in array of Tokens
 * @return false if Syntax Error appeared, true if success
 */
bool typeCheck(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);

/**
 * @brief check if fuction type is void or t_type (int | float | string) or t_nullType (?int | ?string | ?float)
 * @param list list of Tokens
 * @param index index in array of Tokens
 * @return false if Syntax Error appeared, true if success 
 */
bool functionType(SYN_ANALYZER_TYPE_N_PARAM_PARAMS);

// <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
bool functionDeclare(TokenList *list, int *index, stack_ast_t *stackSyn);
// <st-list> -> <stat> <st-list> || eps
bool statList(SYN_ANALYZER_PARAMS);

// if ( <expr> ) { <st-list> } else { <st-list> }
bool statementIf(SYN_ANALYZER_PARAMS);
// while ( <expr> ) { <st-list> }
bool statementWhile(SYN_ANALYZER_PARAMS);
// return <expr> ; || return;
bool statementReturn(SYN_ANALYZER_PARAMS);
// <assign> -> <var> <r-side>
// parse variables to symtable
bool statementVariable(SYN_ANALYZER_PARAMS);
// <stat> -> if || while || return || assign
bool statement(SYN_ANALYZER_PARAMS);

// <seq-stats> -> <stat> <seq-stats> || <fnc-decl> <seq-stats> || eps
bool seqStats(SYN_ANALYZER_PARAMS);
// <prog> -> <prolog> <seq-stats> <epilog>
bool checkSyntax(SYN_ANALYZER_PARAMS);

/**
 * @brief main program for parser, initializing needed sources and start descent recursion
 * 
 * @param list list of Tokens
 * @return SyntaxItem* 
 */
SyntaxItem *synAnalyser(TokenList *list);

#endif // IFJ_SYN_ANALYZER_H
