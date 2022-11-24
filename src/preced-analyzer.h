#ifndef IFJ_PRECED_ANALYZER_H
#define IFJ_PRECED_ANALYZER_H 1

#include "lex-analyzer.h"

#include "stack.h"

#include <stdbool.h>

PrecedItem *stack_precedence_top_terminal(stack_precedence_t *stack);

void Ei(stack_precedence_t *stack, stack_ast_t *stackAST, ht_table_t *symtable);

void opE(stack_precedence_t *stack);

void minusE(stack_precedence_t *stack, stack_ast_t *stackAST);

void EopE(stack_precedence_t *stack, stack_ast_t *stackAST, AST_type op);

void callReductionRule(stack_precedence_t *stack, stack_ast_t *stackAST, int ruleNum, ht_table_t *symtable);

bool reduce(stack_precedence_t *stack, stack_ast_t *stackAST, ht_table_t *symtable);

Element getIndex(Token *input, ht_table_t* symtable);

PrecedItem *precedItemCtor(Token *token, Element type);

bool freeStack(stack_precedence_t *stack, stack_ast_t *stack2, bool returnValue);

bool parseFunctionCall(TokenList *list, int *index,stack_precedence_t *stack, stack_ast_t *stackAST, ht_table_t *symtable);

bool parseExpression(TokenList *list, int *index, ht_table_t *symtable, stack_ast_t *stackAST);



#endif // IFJ_SYN_ANALYZER_H
