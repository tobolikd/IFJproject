#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "error-codes.h"
#include "symtable.h"
#include "preced-analyzer.h"
#include "stack.h"
#include <stdlib.h>

//PRECEDENCE TABLE
const char preced_table[18][18] = 
{
//      {'!', '*', '/','+', '-', '.', '<', '>','>=','<=','==','!=', '(', ')', 'i', '$'          #priority
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // !   #0
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // *   #1
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // /   #1
        {'<', '<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // +   #2
        {'<', '<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // -   #2
        {'<', '<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // .   #2 
        {'<', '<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // >   #3
        {'<', '<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // <   #3
        {'<', '<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // >=  #3
        {'<', '<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // <=  #3
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', ' ', ' ', '<', '>', '<', '>'}, // === #4
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', ' ', ' ', '<', '>', '<', '>'}, // !== #4 
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' '}, // (
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // )
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // i
        {'<', '<', '<','<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', 'x'}  // $
};

//RULES
//                     <| ===
//                  READ BACKWARDS
unsigned const RULES[NUMBER_OF_RULES][RULE_SIZE] = {
    {EXPRESSION, UNINITIALISED, UNINITIALISED},     // E -> E
    {DATA, UNINITIALISED, UNINITIALISED},           // E -> i
    {EXPRESSION, OPERATOR_PLUS, UNINITIALISED},     // E -> -E 
    {EXPRESSION, OPERATOR_MINUS, UNINITIALISED},    // E -> +E
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
    {RIGHT_BRACKET, EXPRESSION, LEFT_BRACKET}       // E -> (E) 
};//possibly more

//HEADER

/// @brief Analyses syntax in expression. Creates AST.
/// @param List Input tokens.
/// @param index Array index.
/// @return NULL if syntax error.
/// @return Pointer to AST.
bool parseExpression(TokenList *List, int *index);

PrecedItem *stack_precedence_top_terminal(stack_precedence_t *stack);

PrecedItem *callReductionRule(stack_precedence_t *stack, PrecedItem **itemArr, unsigned ruleNum);

bool reduce(stack_precedence_t *stack);

Element getIndex(Token *input);

PrecedItem *precedItemCtor(Token *token, Element type);

void precedItemDtor(PrecedItem *item);

bool parseExpression(TokenList *list, int * index);



//FUNCTIONS

PrecedItem *stack_precedence_top_terminal(stack_precedence_t *stack)
{
    stack_precedence_item_t *cur = stack->top;
    while (cur->data->element == EXPRESSION)
        if (cur->next != NULL)
            cur = cur->next;
        else
            return NULL;
        
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
/// @brief 
/// @param stack 
/// @return TRUE IS CORRECT 
bool reduce(stack_precedence_t *stack)
{
    //init array
    PrecedItem *itemArr[RULE_SIZE];
    for (unsigned i = 0; i < RULE_SIZE; i++)
        itemArr[i] = NULL;
    
    unsigned curRule[RULE_SIZE] = {UNINITIALISED, UNINITIALISED, UNINITIALISED};
    unsigned index = 0;
    
    stack_precedence_item_t *item = stack->top; //top item of stack 

    while (item->data->reduction == false && index <= RULE_SIZE) // Get all elemets until < sign
    {
        curRule[index] = item->data->element; 
        itemArr[index] = item->data;  // RULE IS <--- DIRECTION (backwards) 
        if (item->next == NULL)
        {
            debug_log("PA: Expression overreached.\n");
            return false;
        }
        //next
        item = item->next;        
        index++;
    }
    if (index > RULE_SIZE)
        /* ERROR */
        return false;

    if (item->data->element != DOLLAR)
        item->data->reduction = false; //dealt with ... 

    //Compare & call rule
    for (unsigned i = 0; i < NUMBER_OF_RULES ; i++)
        for (unsigned j = 0; j < RULE_SIZE; j++)
        {
            if (curRule[j] != RULES[i][j]) //not this rule
                break;
            if (j == RULE_SIZE-1) //coplete rule
            {
                //reduce
                // callReductionRule(stack,itemArr,j);
                for (int k = 0; k < index; k++) //pop all used elements
                    stack_precedence_pop(stack); 

                stack_precedence_push(stack,precedItemCtor(NULL,EXPRESSION));   //push reduced
                return true;
            }
        }
    return false;
}

Element getIndex(Token *input)
{
    if (input == NULL)
        return UNINITIALISED;
    
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
            case '/':
                return OPERATOR_DIVIDE;
            case '.':
                return OPERATOR_CONCAT;
            default:
            /* LEXICAL ERROR */
                return UNINITIALISED; 
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
            return UNINITIALISED;

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
        case t_lPar:
            return LEFT_BRACKET;
        case t_rPar:
            return RIGHT_BRACKET;
        default:
            /* SYNTAX ERROR */
            return UNINITIALISED;
    }    
}

PrecedItem *precedItemCtor(Token *token, Element type)
{
    PrecedItem *newItem = (PrecedItem*)malloc(sizeof(PrecedItem));
    CHECK_MALLOC_PTR(newItem);
    newItem->element= type;
    newItem->reduction= false;
    newItem->token= token;
    return newItem;
}

void freeStack(stack_precedence_t *stack)
{
    
    while (!stack_precedence_empty(stack))
        stack_precedence_pop(stack);
}

//for now returns boolean
bool parseExpression(TokenList *list, int *index)
{
    // INIT STACK
    stack_precedence_t stack;
    stack_precedence_init(&stack);
    stack_precedence_push(&stack,precedItemCtor(NULL,DOLLAR)); //push starting tokent
    stack.top->data->reduction = true; //set to close

    PrecedItem *topItem;
    Element curInputIndex;
    Token * curInputToken;

    while (1)
    {

        topItem = stack_precedence_top_terminal(&stack);
        if (topItem == NULL)
        { /* ERROR - there no terminal */
            debug_log("PA: No terminal on stack.\n");
            freeStack(&stack);
            return false;
        }
        
        curInputToken = list->TokenArray[(*index)]; //read token from array

        curInputIndex = getIndex(curInputToken);
        if (curInputIndex == 0)
        { /* ERROR */
            if (topItem->element == DOLLAR) //nothing left but non terminal
            {
                freeStack(&stack);
                return true;
            }
            if (reduce(&stack)) //if reducable
                continue;
            debug_log("PA: Invalid expression. Line: %d.\n",curInputToken->lineNum);
            freeStack(&stack);
            return false;
        }    
    
        switch (preced_table[topItem->element][curInputIndex]) //search preced table
        {
        case '=':
            stack_precedence_push(&stack,precedItemCtor(curInputToken,curInputIndex));
            *index +=1; //get another token
            break;

        case '<':
            topItem->reduction = true;
            stack_precedence_push(&stack,precedItemCtor(curInputToken,curInputIndex));
            *index +=1; //get another token
            break;
        case '>':
            if(!reduce(&stack))
            {
                debug_log("PA: Invalid expression. Line: %d.\n",curInputToken->lineNum);
                freeStack(&stack);
                return false;
            }
            break; //input token stays, // look at another top item
        
        default:
            /* SYNTAX ERROR */
            debug_log("PA: Invalid expression. Line: %d.\n",curInputToken->lineNum);
            freeStack(&stack);
            return false;
            break;
        }
    }
    
    freeStack(&stack);
    return true;
}

