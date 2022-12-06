/* @file preced_analyzer_data.h
 *
 * @brief Data structures for bottom-up parser module.
 *
 * @author Gabriel Biel(xbielg00)
 */

#ifndef IFJ_PRECED_ANALYZER_DATA_H
#define IFJ_PRECED_ANALYZER_DATA_H 1

#include "lex_analyzer.h"

#include <stdbool.h>

#define NUMBER_OF_RULES 16
#define RULE_SIZE 3

typedef enum {
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
    DOLLAR,
    UNINITIALISED,
    EXPRESSION
}Element;

typedef struct 
{
    Element element;
    Token *token;
    bool reduction;
}PrecedItem;


#endif // IFJ_PRECED_ANALYZER_DATA_H
