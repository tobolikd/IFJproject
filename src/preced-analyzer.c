#include "preced-analyzer.h"
#include "lex-analyzer.h"
#include "error-codes.h"
#include "symtable.h"

char get_op(Token *input)
{
switch (input->type)
    {
    case t_operator:
        switch (input->data[0])
        {
        case '+':
            return OPERATOR_PLUS;
        case '-':
            return OPERATOR_MINUS;
        case '*':
            return OPERATOR_MULTIPLY;
        case '\\':
            return OPERATOR_DIVIDE;
        case '.':
            return OPERATOR_CONCAT;
        default:
        /* LEXICAL ERROR */
            return '\0'; 
        }
        break;
    
    case t_comparator:
        if (!strcmp(input->data,"==="))
            return OPERATOR_E;
        if (!strcmp(input->data,"!=="))
            return OPERATOR_NE;
        if (!strcmp(input->data,"<="))
            return OPERATOR_LE;
        if (!strcmp(input->data,">="))
            return OPERATOR_GE;
        if (!strcmp(input->data,">"))
            return OPERATOR_LT;
        if (!strcmp(input->data,"<"))
            return OPERATOR_GT;

    case t_varId:
        //check in symtable
        // if(ht_search(symTable,input->data) == NULL)
        //     THROW_ERROR(5,input->lineNum);
        return DATA;//_VAR

    case t_int:
        return DATA;//_INT
    case t_float:
        return DATA;//_FLOAT
    case t_string:
        return DATA;//_STRING

    default:
        /* SYNTAX ERROR */
        return '\0';
    }    
}

//for now returns boolean
bool parseExpression(TokenList *List, int * index)
{
    while (1)
    {
        Token *input = List->TokenArray[*index];
        if (*index == List->length)
            return false;
        
        *index = *index +1;
    }
}

