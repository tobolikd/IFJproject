#ifndef IFJ_PRECED_ANALYZER_H
#define IFJ_PRECED_ANALYZER_H 1


#include <stdbool.h>

#define NUMBER_OF_RULES 16
#define RULE_SIZE 3

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
    DOLLAR,
    EXPRESSION
}Element;

typedef struct 
{
    Element element;
    Token *token;
    bool reduction;
}PrecedItem;


#endif // IFJ_SYN_ANALYZER_H