#include "lex-analyzer.h"
#include "symtable.h"
#include "preced-analyzer.h"
#include "ast.h"
#include "stack.h"
#include <stdlib.h>

//PRECEDENCE TABLE
const char preced_table[EXPRESSION][EXPRESSION] =   //experssion is the last in enum, 
                                                    //enum contains one extra element UNINITIALISED
{
//      {'!', '*', '/','+', '-', '.', '<', '>','>=','<=','==','!=', '(', ')', 'i', '$'          #priority
        {'>', '>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // !   #0 = not in specification
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
    {DATA, UNINITIALISED, UNINITIALISED},           // 0 E -> i
    {EXPRESSION, OPERATOR_PLUS, UNINITIALISED},     // 1 E -> -E 
    {EXPRESSION, OPERATOR_MINUS, UNINITIALISED},    // 2 E -> +E
    {EXPRESSION, OPERATOR_PLUS, EXPRESSION},        // 3 E -> E+E
    {EXPRESSION, OPERATOR_MINUS, EXPRESSION},       // 4 E -> E-E
    {EXPRESSION, OPERATOR_MULTIPLY, EXPRESSION},    // 5 E -> E*E    
    {EXPRESSION, OPERATOR_DIVIDE, EXPRESSION},      // 6 E -> E/E
    {EXPRESSION, OPERATOR_CONCAT, EXPRESSION},      // 7 E -> E.E
    {EXPRESSION, OPERATOR_LT, EXPRESSION},          // 8 E -> E<E
    {EXPRESSION, OPERATOR_GT, EXPRESSION},          // 9 E -> E>E
    {EXPRESSION, OPERATOR_LE, EXPRESSION},          //10 E -> E<=E
    {EXPRESSION, OPERATOR_GE, EXPRESSION},          //11 E -> E>=E
    {EXPRESSION, OPERATOR_E, EXPRESSION},           //12 E -> E===E
    {EXPRESSION, OPERATOR_NE, EXPRESSION},          //13 E -> E!==E
    {RIGHT_BRACKET, EXPRESSION, LEFT_BRACKET}       //14 E -> (E) 
};//possibly more

//HEADER

/// @brief Analyses syntax in expression. Creates AST.
/// @param List Input tokens.
/// @param index Array index.
/// @return NULL if syntax error.
/// @return Pointer to AST.
bool parseExpression(TokenList *List, int *index, ht_table_t *symtable);

PrecedItem *stack_precedence_top_terminal(stack_precedence_t *stack);

void callReductionRule(stack_precedence_t *stack, stack_ast_t *stackAST, int ruleNum);

bool reduce(stack_precedence_t *stack, stack_ast_t *stackAST);

Element getIndex(Token *input);

PrecedItem *precedItemCtor(Token *token, Element type);

void precedItemDtor(PrecedItem *item);

bool parseExpression(TokenList *list, int *index, ht_table_t *symtable);



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

void Ei(stack_precedence_t *stack, stack_ast_t *stackAST)
 {
    //setup stack_precedence_t
    PrecedItem *item = stack_precedence_top(stack);
    item->element = EXPRESSION;

    //setup stack_ast_t
    switch (item->token->type)
    {
    case t_varId:
        stack_ast_push(stackAST,ast_item_const(AST_VAR,NULL));
        break;
    case t_functionId:
        stack_ast_push(stackAST,ast_item_const(AST_FUNCTION_CALL,NULL));
        break;
    case t_int:
    {
        int data = atoi(item->token->data);
        stack_ast_push(stackAST,ast_item_const(AST_INT, &data));
        break;
    }
    case t_float:
    {
        double data = atof(item->token->data);
        stack_ast_push(stackAST,ast_item_const(AST_FLOAT, &data));
        break;
    }
    case t_string:
        stack_ast_push(stackAST,ast_item_const(AST_STRING,item->token->data));
        break;
    default:
        THROW_ERROR(SEMANTIC_OTHER_ERR, item->token->lineNum);
        break;
    }
}

void opE(stack_precedence_t *stack)
{
    stack_precedence_item_t *E = stack->top;
    stack_precedence_item_t *deleted = stack->top->next;
    E->next = deleted->next;
    deleted->next = E;
    stack->top = deleted;
    stack_precedence_pop(stack);// pop sign operator
}


void minusE(stack_precedence_t *stack, stack_ast_t *stackAST)
{
    //handle semantic action for E -> -E
    AST_item *item = stack_ast_top(stackAST);
    switch (item->type)
    {
    case AST_FLOAT:
        item->data->floatValue *= -1;
        break;
    case AST_INT:
        item->data->intValue *= -1;
        break;
    case AST_STRING:
        THROW_ERROR(SEMANTIC_RUN_TYPE_ERR,stack->top->data->token->lineNum);
        break;
    case AST_VAR:
    case AST_FUNCTION_CALL:
    {
        //push operation * (-1)
        int negative = -1;
        stack_ast_push(stackAST,ast_item_const(AST_INT,&negative));
        stack_ast_push(stackAST,ast_item_const(AST_MULTIPLY,NULL)); //push operation multiply
        break;
    }
    default://will most likely never happen
        THROW_ERROR(SEMANTIC_OTHER_ERR,stack->top->data->token->lineNum);
        break;
    }
    //handle E -> -E
    opE(stack);
}

void EopE(stack_precedence_t *stack, stack_ast_t *stackAST, AST_type op)
{
    stack_ast_push(stackAST,ast_item_const(op, NULL));
    stack_precedence_pop(stack);
    stack_precedence_pop(stack);
}


void callReductionRule(stack_precedence_t *stack, stack_ast_t *stackAST, int ruleNum)
{
    switch (ruleNum)
    {
    case 0://E -> i
        Ei(stack,stackAST);
        break;
    case 1://E -> -E
        minusE(stack,stackAST);
        break;
    case 2://E -> +E
        opE(stack);
        break;
    case 3:// E -> E+E
        EopE(stack,stackAST,AST_ADD);
        break;
    case 4:// E -> E-E
        EopE(stack,stackAST,AST_SUBTRACT);
        break;
    case 5:// E -> E*E 
        EopE(stack,stackAST,AST_MULTIPLY);
        break;
    case 6:// E -> E/E
        EopE(stack,stackAST,AST_DIVIDE);
        break;
    case 7:// E -> E.E
        EopE(stack,stackAST,AST_CONCAT);
        break;
    case 8:// E -> E<E
        EopE(stack,stackAST,AST_LESS);
        break;
    case 9:// E -> E>E
        EopE(stack,stackAST,AST_GREATER);
        break;
    case 10:// E -> E<=E
        EopE(stack,stackAST,AST_LESS_EQUAL);
        break;
    case 11:// E -> E>=E
        EopE(stack,stackAST,AST_GREATER_EQUAL);
        break;
    case 12:// E -> E===E
        EopE(stack,stackAST,AST_EQUAL);
        break;
    case 13:// E -> E!==E
        EopE(stack,stackAST,AST_NOT_EQUAL);
        break;
    case 14:// E -> (E) 
    {
        // no semantic action just reduce stack
        stack_precedence_pop(stack);
        //change order & pop
        stack_precedence_item_t *tmp = stack->top->next;
        stack->top->next = stack->top->next->next;
        tmp->next = stack->top;
        stack->top = tmp;
        stack_precedence_pop(stack);
        break;
    }
    default:
        debug_print("PA: Could not assign rule.\n");
        break;
    }

    stack_precedence_top(stack)->token = NULL;  // token is irelevant at this point
                                                // data type may have changed 
    return;
}

/// @brief 
/// @param stack 
/// @return TRUE IS CORRECT 
bool reduce(stack_precedence_t *stack, stack_ast_t *stackAST)
{
    unsigned curRule[RULE_SIZE] = {UNINITIALISED, UNINITIALISED, UNINITIALISED};
    unsigned index = 0;
    
    stack_precedence_item_t *item = stack->top; //top item of stack 

    while (item->data->reduction == false && index <= RULE_SIZE) // Get all elemets until < sign
    {
        curRule[index] = item->data->element; // RULE IS <--- DIRECTION (backwards) 
        if (item->next == NULL)
        {
            debug_print("PA: Expression overreached.\n");
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
                debug_log("Found rule number: %d \n", i);
                //reduce
                callReductionRule(stack, stackAST, i);
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

        case t_functionId:
            //check in fnc symtable
            //recursive syntax check
            return DATA;

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
            return DOLLAR;//closing tak of expression 
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

void freeStack(stack_precedence_t *stack, stack_ast_t *stack2)
{    
    while (!stack_precedence_empty(stack))
        stack_precedence_pop(stack);



    while (!stack_ast_empty(stack2))
    {
        debug_log("DELETING: stack ast type: %d\n",stack_ast_top(stack2)->type);
        stack_ast_pop(stack2);
    }


}

//for now returns boolean
bool parseExpression(TokenList *list, int *index, ht_table_t *symtable)
{
    // INIT STACK
    stack_precedence_t stack;
    stack_precedence_init(&stack);
    stack_precedence_push(&stack,precedItemCtor(NULL,DOLLAR)); //push starting tokent
    stack.top->data->reduction = true; //set to close
    
    stack_ast_t stackAST;
    stack_ast_init (&stackAST);

    PrecedItem *topItem;
    Element curInputIndex;
    Token * curInputToken;

    while (1)
    {
        topItem = stack_precedence_top_terminal(&stack);
        if (topItem == NULL)
        { /* ERROR - there no terminal */
            debug_print("PA: No terminal on stack.\n");
            freeStack(&stack, &stackAST);
            return false;
        }
        
        curInputToken = list->TokenArray[(*index)]; //read token from array

        curInputIndex = getIndex(curInputToken);
        if (curInputIndex == UNINITIALISED)
        {
            debug_print("PA: Failed getting index. Line: %d.\n",curInputToken->lineNum);
            freeStack(&stack, &stackAST);
            return false;
        }
        // debug_log("Working on #%d element: %d\n",(*index),curInputIndex);
        
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
            if(!reduce(&stack, &stackAST))
            {

                debug_print("PA: Invalid expression. Line: %d.\n",curInputToken->lineNum);
                freeStack(&stack, &stackAST);
                return false;
            }
            break; //input token stays the same, // look at another top terminal

        case 'x': // $ __ $ //no terminal left but $
            freeStack(&stack, &stackAST);
            return true;

        default:
            /* SYNTAX ERROR */
            debug_print("PA: Invalid expression. Line: %d.\n",curInputToken->lineNum);
            freeStack(&stack, &stackAST);
            return false;
            break;
        }
    }
    
    freeStack(&stack, &stackAST);
    return true;
}

