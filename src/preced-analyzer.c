#include "preced-analyzer.h"
#include "lex-analyzer.h"
#include "error-codes.h"
#include "symtable.h"
#include "stack.h"

#include <stdlib.h>


PrecedItem *stack_precedence_top_terminal(stack_precedence_t *stack)
{
    stack_precedence_item_t *cur = stack->top;
    while (cur->data->element == EXPRESSION)
        cur = cur->next;
    return cur->data;
}

PrecedItem *callReductionRule
    (stack_precedence_t *stack, PrecedItem **itemArr, unsigned ruleNum)
{
    switch (ruleNum)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        break;
    case 9:
        break;
    case 10:
        break;
    case 11:
        break;
    case 12:
        break;
    case 13:
        break;
    case 14:
        break;
    
    default:
        break;
    }
    return NULL;
}

bool reduce(stack_precedence_t *stack)
{
    //init array
    PrecedItem *itemArr[RULE_SIZE];
    for (unsigned i = 0; i < RULE_SIZE; i++)
        itemArr[i] = NULL;
    
    unsigned curRule[RULE_SIZE] = {UNINITIALISED, UNINITIALISED, UNINITIALISED};
    unsigned index = 0;
    
    stack_precedence_item_t *item = stack->top; //top item of stack 

    while (item->data->reduction == false)
    {
        curRule[index] = item->data->element; 
        itemArr[index] = item->data;  // RULE IS <--- DIRECTION (backwards) 
        //next
        item = item->next;        
        index++;
    }

    if (index > RULE_SIZE)
        /* ERROR */
        return;

    //Find & call rule
    for (unsigned i = 0; i < NUMBER_OF_RULES ; i++)
        for (unsigned j = 0; j < RULE_SIZE; j++)
            if (curRule[j] == RULES[i][j])
            {
                PrecedItem * E = callReductionRule(stack,itemArr,stack);
                //reduce
                for (unsigned i = 0; i <= index; i++)
                    stack_precedence_pop(stack); 

                stack_precedence_push(stack,E); 
            }
    

}

char getIndex(Token *input)
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

PrecedItem *precedItemCtor(Token *token, Element type)
{
    PrecedItem *newItem = malloc(sizeof(PrecedItem));
    CHECK_MALLOC_PTR(newItem);
    newItem->element= type;
    newItem->reduction= false;
    newItem->token= token;
    return newItem;
}

void precedItemDtor(PrecedItem *item)
{
    free(item);
}

//for now returns boolean
bool parseExpression(TokenList *list, int * index)
{
    // INIT STACK
    stack_precedence_t *stack;
    stack_precedence_init(stack);
    // PUSH $ 
    PrecedItem *topItem;
    topItem->element= CLOSING_TAG;
    topItem->reduction= false;
    topItem->token= NULL;
    stack_precedence_push(stack,topItem); //push starting tokent

    Element curInputIndex;
    Token * curInputToken;
    while (1)
    {

        topItem = stack_precedence_top_terminal(stack);
        curInputToken = list->TokenArray[*index];
        curInputIndex = getIndex(curInputToken);


        switch (preced_table[topItem->element][curInputIndex])
        {
        case '=':
            stack_precedence_push(stack,precedItemCtor(curInputToken,curInputIndex));
            *index++; //get another token
            break;

        case '<':
            topItem->reduction = true;
            stack_precedence_push(stack,precedItemCtor(curInputToken,curInputIndex));
            *index++; //get another token
            break;
        case '>':
            if(!reduce(stack))
                return false;
            break; //input token stays, // look at another top item
        
        default:
            /* SYNTAX ERROR */
            return false;
            break;
        }
    }
    return true;
}

