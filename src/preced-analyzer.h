#ifndef IFJ_PRECED_ANALYZER_H
#define IFJ_PRECED_ANALYZER_H 1

#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "error-codes.h"
#include "symtable.h"
#include "stack.h"
#include "stack.c"

#include <stdbool.h>

#define NUMBER_OF_RULES 14
#define RULE_SIZE 3

const char preced_table[18][18] = 
{
//      {'!', '*', '/','+', '-', '.', '<', '>','>=','<=','==','!=', '(', ')', 'i', '$'          #priority
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // !   #0
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // *   #1
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // /   #1
        {'<', '<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // +   #2
        {'<', '<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // -   #2
        {'<', '<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // .   #2 
        {'<', '<', '<','<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // >   #3
        {'<', '<', '<','<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // <   #3
        {'<', '<', '<','<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // >=  #3
        {'<', '<', '<','<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // <=  #3
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '<', '>'}, // === #4
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '<', '>'}, // !== #4 
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' '}, // (
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // )
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // i
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', 'x'}  // $
};


typedef enum {
    UNINITIALISED,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_CONCAT,
    OPERATOR_GT,
    OPERATOR_LT,
    OPERATOR_GE,
    OPERATOR_LE,
    OPERATOR_E,
    OPERATOR_NE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    DATA,
    CLOSING_TAG,
    EXPRESSION
}Element;

typedef struct item_t
{
    Element element;
    Token *token;
    bool reduction;
}PrecedItem;

//RULES

unsigned const RULES[NUMBER_OF_RULES][RULE_SIZE] = {
    {EXPRESSION},                                   // E -> E
    {DATA},                                         // E -> i
    {EXPRESSION, OPERATOR_PLUS, EXPRESSION},        // E -> E+E
    {EXPRESSION, OPERATOR_MINUS, EXPRESSION},       // E -> E-E
    {EXPRESSION, OPERATOR_MULTIPLY, EXPRESSION},    // E -> E*E    
    {EXPRESSION, OPERATOR_DIVIDE, EXPRESSION},      // E -> E/E
    {EXPRESSION, OPERATOR_CONCAT, EXPRESSION},      // E -> E.E
    {EXPRESSION, OPERATOR_LT, EXPRESSION},          // E -> E<E
    {EXPRESSION, OPERATOR_GT, EXPRESSION},          // E -> E>E
    {EXPRESSION, OPERATOR_LE, EXPRESSION},          // E -> E<=E
    {EXPRESSION, OPERATOR_GE, EXPRESSION},          // E -> E>=E
    {EXPRESSION, OPERATOR_E, EXPRESSION},           // E -> E===E
    {EXPRESSION, OPERATOR_NE, EXPRESSION},          // E -> E!==E
    {LEFT_BRACKET, EXPRESSION, RIGHT_BRACKET}       // E -> (E)
};//possibly more



/// @brief Analyses syntax in expression. Creates AST.
/// @param List Input tokens.
/// @param index Array index.
/// @return NULL if syntax error.
/// @return Pointer to AST.
bool parseExpression(TokenList *List, int *index);

PrecedItem *stack_precedence_top_terminal(stack_precedence_t *stack);

PrecedItem *callReductionRule(stack_precedence_t *stack, PrecedItem **itemArr, unsigned ruleNum);

bool reduce(stack_precedence_t *stack);

char getIndex(Token *input);

PrecedItem *precedItemCtor(Token *token, Element type);

void precedItemDtor(PrecedItem *item);

bool parseExpression(TokenList *list, int * index);


#endif // IFJ_SYN_ANALYZER_H
