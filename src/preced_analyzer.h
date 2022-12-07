/**
 * IFJ22 compiler
 *
 * @file preced_analyzer.h
 * 
 * @brief library for bottom-up parser module
 *
 * @author Gabriel Biel(xbielg00)
 */

#ifndef IFJ_PRECED_ANALYZER_H
#define IFJ_PRECED_ANALYZER_H 1

#include "lex_analyzer.h"

#include "stack.h"

#include <stdbool.h>

/// @brief Checks syntax and semantics of function call. 
/// @param list List of tokens.
/// @param index Current position in list of tokens.
/// @param stack Stack for expression parsing.
/// @param stackAST Where to push AST items.
/// @param symtable Relevant variable symtable for current frame.
/// @return True if function call is valid.
bool parseFunctionCall(TokenList *list, int *index,stack_precedence_t *stack, stack_ast_t *stackAST, ht_table_t *symtable);

/// @brief Checks syntax and semantics of expression.
/// @param list List of tokens.
/// @param index Current position in list of tokens.
/// @param symtable Relevant variable symtable for currennt frame.
/// @param stackAST Where to push AST items.
/// @return True if expression is valid.
bool parseExpression(TokenList *list, int *index, ht_table_t *symtable, stack_ast_t *stackAST);

#endif // IFJ_SYN_ANALYZER_H
