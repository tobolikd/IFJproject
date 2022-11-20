#ifndef IFJ_PRECED_ANALYZER_H
#define IFJ_PRECED_ANALYZER_H 1

#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "error-codes.h"
#include "symtable.h"

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
    OPERATOR_NOT,
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
    INPUT,
    CLOSING_TAG,
    EXPRESSION
}Element;

//RULES

unsigned const RULES[14][3] = {
    {EXPRESSION},                                   // E -> E
    {INPUT},                                        // E -> i
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
bool precedAnalyzer(TokenList *List, int index);


#endif // IFJ_SYN_ANALYZER_H