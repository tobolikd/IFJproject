#include "symtable.h"
#include "preced_analyzer_data.h"
#include "preced_analyzer.h"
#include "sem-analyzer.h"
#include "ast.h"
#include "error-codes.h"
#include <stdlib.h>

//PRECEDENCE TABLE
const char preced_table[EXPRESSION][EXPRESSION] =   //experssion is the last in enum,
                                                    //enum contains one extra element UNINITIALISED
{
//      {'*', '/','+', '-', '.', '<', '>','>=','<=','==','!=', '(', ')', 'i', '$'          #priority
        {'>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // *   #1
        {'>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // /   #1
        {'<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // +   #2
        {'<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // -   #2
        {'<', '<','>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // .   #2
        {'<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // >   #3
        {'<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // <   #3
        {'<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // >=  #3
        {'<', '<','<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '<', '>', '<', '>'}, // <=  #3
        {'<', '<','<', '<', '<', '<', '<', '<', '<', ' ', ' ', '<', '>', '<', '>'}, // === #4
        {'<', '<','<', '<', '<', '<', '<', '<', '<', ' ', ' ', '<', '>', '<', '>'}, // !== #4
        {'<', '<','<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' '}, // (
        {'>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // )
        {'>', '>','>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // i
        {'<', '<','<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', ' '}  // $
};

//RULES
//                     <| ===
//                  READ BACKWARDS
unsigned const RULES[NUMBER_OF_RULES][RULE_SIZE] = {
    {EXPRESSION, OPERATOR_PLUS, UNINITIALISED},     // 0 E -> +E
    {EXPRESSION, OPERATOR_MINUS, UNINITIALISED},    // 1 E -> -E
    {DATA, UNINITIALISED, UNINITIALISED},           // 2 E -> i
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
    {RIGHT_BRACKET, EXPRESSION, LEFT_BRACKET},      //14 E -> (E)
    {EXPRESSION, UNINITIALISED, UNINITIALISED}      //15 E -> E
};//possibly more


/// @brief Skips expressions in precedence stack.
/// @param stack Stack to read from.
/// @return Pointer to the terminal item that is closest to the top.
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

/// @brief Application of precedence rule E -> i
/// @param stack Precedence stack.
/// @param stackAST AST stack to push items to.
/// @param symtable Relevant symtable for current expression.
void Ei(stack_precedence_t *stack, stack_ast_t *stackAST, ht_table_t *symtable)
{
    //setup stack_precedence_t
    PrecedItem *item = stack_precedence_top(stack);
    item->element = EXPRESSION;

    //setup stack_ast_t
    switch (item->token->type)
    {
    case t_varId:
        stack_ast_push(stackAST,ast_item_const(AST_VAR,ht_search(symtable,stack_precedence_top(stack)->token->data)));
        break;
    case t_functionId://dealt with earlier
        debug_log("PA: E -> i rule something went wrong.\n");
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
    case t_null:
        stack_ast_push(stackAST,ast_item_const(AST_NULL,NULL));
        break;
    default:
        THROW_ERROR(SEMANTIC_OTHER_ERR, item->token->lineNum);
        break;
    }
}

/// @brief Application of precedence rule E -> -+E
/// @param stack Precedence stack.
/// @param stackAST AST stack to push items to.
void opE(stack_precedence_t *stack)
{
    stack_precedence_item_t *E = stack->top;
    stack_precedence_item_t *deleted = stack->top->next;
    E->next = deleted->next;
    deleted->next = E;
    stack->top = deleted;
    stack_precedence_pop(stack);// pop sign operator
}


/// @brief Application of precedence rule E -> -E
/// @param stack Precedence stack.
/// @param stackAST AST stack to push items to.
void minusE(stack_precedence_t *stack, stack_ast_t *stackAST)
{
    //handle semantic action for E -> -E
    AST_item *item = stack_ast_top(stackAST);
    if (item == NULL)
    {
        debug_log("PA: -E rule failed. Stack Empty.\n");
        return;
    }

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
    default:
        {
        //push operation * (-1)
        int negative = -1;
        stack_ast_push(stackAST,ast_item_const(AST_INT,&negative));
        stack_ast_push(stackAST,ast_item_const(AST_MULTIPLY,NULL)); //push operation multiply
        break;
        }
    }
    //handle E -> -E
    opE(stack);
}

/// @brief Application of precedence rule E -> E {operand} E
/// @param stack Precedence stack.
/// @param stackAST AST stack to push items to.
/// @param op Type of operand to execute. {+,-,*,\,.,<,>,<=,>=,===,!==}
void EopE(stack_precedence_t *stack, stack_ast_t *stackAST, AST_type op)
{
    stack_ast_push(stackAST,ast_item_const(op, NULL));
    stack_precedence_pop(stack);
    stack_precedence_pop(stack);
}

/// @brief Executes found rule.
/// @param ruleNum Rule to be executed. 
void callReductionRule(stack_precedence_t *stack, stack_ast_t *stackAST, int ruleNum, ht_table_t *symtable)
{
    switch (ruleNum)
    {
    case 0://E -> +E --> ERROR
        opE(stack);
        break;
    case 1://E -> -E --> ERROR
        minusE(stack,stackAST);
        break;
    case 2://E -> i
        Ei(stack,stackAST,symtable);
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
    case 15:// E -> E
        break;

    default:
        THROW_ERROR(SYNTAX_ERR,0);
        debug_print("PA: Could not assign rule.\n");
        break;
    }

    stack_precedence_top(stack)->token = NULL;  // token is irelevant at this point
                                                // data type may have changed
    return;
}

/// @brief Reduces the current expression on precedence stack.
/// @return True if expression is ruducable.
/// @return False if rule for expression is nout found.
bool reduce(stack_precedence_t *stack, stack_ast_t *stackAST, ht_table_t *symtable)
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
    for (unsigned i = 2; i < NUMBER_OF_RULES ; i++)
        for (unsigned j = 0; j < RULE_SIZE; j++)
        {
            if (curRule[j] != RULES[i][j]) //not this rule
                break;
            if (j == RULE_SIZE-1) //coplete rule
            {
                debug_log("Found rule number: %d \n", i);
                //reduce
                callReductionRule(stack, stackAST, i, symtable);
                return true;
            }
        }
    debug_log("Could not find rule.\n");
    return false;
}

/// @brief Checks syntax of expression.
/// @param input Token to be examined.
/// @param symtable Relevant symtable for current frame.
/// @return Element that represents input token. 
Element getIndex(Token *input, ht_table_t* symtable)
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
                return OPERATOR_GT;
            if (!strcmp(input->data,"<"))
                return OPERATOR_LT;
            return UNINITIALISED;

        case t_varId:
            //check if var was declared before
            if (ht_search(symtable,input->data) == NULL) //not in symtable
            {
                THROW_ERROR(SEMANTIC_VARIABLE_ERR,input->lineNum);
                return UNINITIALISED;
            }
            ht_search(symtable,input->data)->referenceCounter++; //variable referenced
            return DATA;//_VAR

        case t_functionId:
            //check if fnc was declared anywhere in the programme
            if (ht_search(fncTable,input->data) == NULL) //not in symtable
            {
                THROW_ERROR(SEMANTIC_FUNCTION_DEFINITION_ERR,input->lineNum);
                debug_print("not initialized function call on line %d\n", input->lineNum);
                return UNINITIALISED;
            }
            return DATA;//_FNC

        case t_int:
            return DATA;//_INT
        case t_float:
            return DATA;//_FLOAT
        case t_string:
            return DATA;//_STRING
        case t_null:
            return DATA;//_null
        case t_lPar:
            return LEFT_BRACKET;
        case t_rPar:
            return RIGHT_BRACKET;
        default:
            return DOLLAR;//closing tag of expression
    }
}

/// @brief Constructor for precence item.
/// @return Pointer to new precendence item. 
PrecedItem *precedItemCtor(Token *token, Element type)
{
    PrecedItem *newItem = (PrecedItem*)malloc(sizeof(PrecedItem));
    CHECK_MALLOC_PTR(newItem);
    newItem->element= type;
    newItem->reduction= false;
    newItem->token= token;
    return newItem;
}

/// @brief Frees stacks based on return value.
/// @param stack Precedence stack, to be freed.
/// @param ast Ast stack that may be freed.
/// @param returnValue Switch for destructing ast stack.
/// @return returnValue.
bool freeStack(stack_precedence_t *stack, stack_ast_t *ast, bool returnValue)
{
    while (!stack_precedence_empty(stack))
        stack_precedence_pop(stack);

    if (returnValue == false) //dispose the whole ast stack
    {
        while (!stack_ast_empty(ast))
        {
            debug_log("DELETING: stack ast type: %d\n",stack_ast_top(ast)->type);
            stack_ast_pop(ast);
        }
    }
    else
        stack_ast_push(ast,ast_item_const(AST_END_EXPRESSION,NULL));
    return returnValue;
}

/// @brief Compares TokenType to var_type_t
/// @return True if they represent the same type. 
bool cmpTHType(TokenType token, var_type_t type)
{
    if (token == t_int && type == int_t)
        return true;
    if (token == t_string && type == string_t)
        return true;
    if (token == t_float && type == float_t)
        return true;
    if (token == t_null && type == void_t)
        return true;
    return false;
}

/// @brief Transfer of var_type_t to AST_param_type
/// @return AST_param_type represented by var_type_t 
AST_param_type HtoAType(var_type_t type)
{
    if (type == int_t)
        return AST_P_INT;
    if (type == float_t)
        return AST_P_FLOAT;
    if (type == string_t)
        return AST_P_STRING;
    if (type == void_t)
        return AST_P_NULL;
    debug_log("Could not match the var_type_t\n");
    return AST_P_NULL;
}

bool parseFunctionCall(TokenList *list, int *index,stack_precedence_t *stack, stack_ast_t *stackAST, ht_table_t *symtable)
{
    //functionId ( <param> ) / Function Call
    // #1 check declaration
    ht_item_t *function = ht_search(fncTable,list->TokenArray[*index]->data);
    if (function == NULL) //function is not declared
    {
        THROW_ERROR(SEMANTIC_FUNCTION_DEFINITION_ERR,list->TokenArray[*index]->lineNum);
        return false;
    }
    // #2 (
    (*index)++;
    if (list->TokenArray[*index]->type != t_lPar)
    {
        THROW_ERROR(SYNTAX_ERR,list->TokenArray[*index]->lineNum);
        return false;
    }
    // #3  matching data type to declaration
    // #3a create AST FUNCTION CALL ITEM
    AST_function_call_data *fncCallData = fnc_call_data_const(fncTable,function->identifier);

    while(1) //check parameter type compared to declaration
    {
        (*index)++;
        if (list->TokenArray[*index]->type == t_rPar)
            break;
        switch (list->TokenArray[*index]->type)
        {
        case t_varId:
            if (ht_search(symtable,list->TokenArray[*index]->data) == NULL) //not in symtable
            {
                fnc_call_data_destr(fncCallData);
                THROW_ERROR(SEMANTIC_VARIABLE_ERR,list->TokenArray[*index]->lineNum);
                return false;
            }
            ht_search(symtable,list->TokenArray[*index]->data)->referenceCounter++; //variable referenced
            //add variable as function parameter
            fnc_call_data_add_param(fncCallData, AST_P_VAR, ht_search(symtable,list->TokenArray[*index]->data));
            break;
        case t_int:
            {
                int intData = atoi(list->TokenArray[*index]->data);
                fnc_call_data_add_param(fncCallData, AST_P_INT, &intData);
            }
            break;
        case t_float:
            {
                double floatData = atof(list->TokenArray[*index]->data);
                fnc_call_data_add_param(fncCallData, AST_P_FLOAT, &floatData);
            }
            break;
        case t_string:
            fnc_call_data_add_param(fncCallData, AST_P_STRING, list->TokenArray[*index]->data);
            break;
        case t_null:
            fnc_call_data_add_param(fncCallData, AST_P_NULL, NULL);
            break;
        default:
            fnc_call_data_destr(fncCallData);
            THROW_ERROR(SYNTAX_ERR,list->TokenArray[*index]->lineNum);
            return false;
        }//end of switch

        (*index)++;
        if (list->TokenArray[*index]->type != t_comma)//does
            break;//                      |
        //if false, it fals through to    V
    } //end of while cycle

    // #4 )
    if (list->TokenArray[*index]->type != t_rPar || list->TokenArray[*index-1]->type == t_comma)
    {
        fnc_call_data_destr(fncCallData);
        THROW_ERROR(SYNTAX_ERR,list->TokenArray[*index]->lineNum);
        return false;
    }
    (*index)++;//move cursor

    // #5 push expression to stack
    if (stack_precedence_top(stack)->element != DOLLAR)
        stack_precedence_top(stack)->reduction = false;
    stack_precedence_push(stack,precedItemCtor(NULL,EXPRESSION));//push expression


    // #6 push semantic action to ast
    if (checkFncCall(fncCallData) == false)// check if sem correct
    {
        fnc_call_data_destr(fncCallData);
        THROW_ERROR(SEMANTIC_RUN_PARAMETER_ERR,list->TokenArray[*index]->lineNum);
        return false;
    }
    stack_ast_push(stackAST,ast_item_const(AST_FUNCTION_CALL,fncCallData));
    return true;//success
}

bool parseExpression(TokenList *list, int *index, ht_table_t *symtable, stack_ast_t *stackAST)
{
    // INIT
    stack_precedence_t stack;
    stack_precedence_init(&stack);
    stack_precedence_push(&stack,precedItemCtor(NULL,DOLLAR)); //push starting tokent
    stack.top->data->reduction = true; //set to close

    PrecedItem *topItem;
    Element curInputIndex;
    Token * curInputToken;

    while (1) //PARSE UNTIL END OF EXPRESSION
    {
        // #1 get top terminal
        topItem = stack_precedence_top_terminal(&stack);
        if (topItem == NULL)
        { /* ERROR - there in no terminal */
            debug_log("PA: Internal error. No terminal on stack.\n");
            return freeStack(&stack, stackAST, false);
        }
        // #2 read token from array
        curInputToken = list->TokenArray[(*index)];

        // #3 get input index to compare in preced table
        curInputIndex = getIndex(curInputToken, symtable); //get index to locate action in preced_table
        if (curInputIndex == UNINITIALISED)//option not found
        {
            debug_print("PA: Failed getting index. Line: %d.\n",curInputToken->lineNum);
            return freeStack(&stack, stackAST, false);
        }

        // #4 if function call do it seperately
        //    can only be reduced to expression and pushed to AST stack
        if (curInputToken->type == t_functionId) //function call
        {
            if (parseFunctionCall(list,index,&stack,stackAST,symtable) == true)//parse function call
                continue;//success
            THROW_ERROR(SYNTAX_ERR,curInputToken->lineNum);
            return freeStack(&stack, stackAST, false);//parsing failed
        }
        // debug_log("Working on #%d element: %d\n",(*index),curInputIndex);

        // #5 make action based on element combination
        switch (preced_table[topItem->element][curInputIndex]) //search preced table
        {
        case '=':
            stack_precedence_push(&stack,precedItemCtor(curInputToken,curInputIndex));
            (*index)++; //get another token
            break;

        case '<':
            topItem->reduction = true;
            stack_precedence_push(&stack,precedItemCtor(curInputToken,curInputIndex));
            (*index)++; //get another token
            break;

        case '>':
            if(!reduce(&stack, stackAST, symtable))
            {
                THROW_ERROR(SYNTAX_ERR,curInputToken->lineNum);
                debug_print("PA: Invalid expression. Line: %d.\n",curInputToken->lineNum);
                return freeStack(&stack, stackAST, false);
            }
            break; //input token stays the same, // look at another top terminal

        default:
            if(curInputIndex == RIGHT_BRACKET || curInputIndex == DOLLAR) //Valid end of expression is)
            {
                if(reduce(&stack, stackAST, symtable)) // if what is on stack is reducable -> reduce
                {
                    //check if stack is reduce only to 1 EXPRESSION and DOLLAR
                    if (stack.top != NULL)
                        if (stack.top->next != NULL)
                            if (stack.top->next->data->element == DOLLAR && stack.top->data->element == EXPRESSION) //must end with
                                return freeStack(&stack, stackAST, true);
                }
            }
            /* SYNTAX ERROR */
            THROW_ERROR(SYNTAX_ERR,curInputToken->lineNum);
            debug_print("PA: Invalid expression. Line number: %d.\n",curInputToken->lineNum);
            return freeStack(&stack, stackAST, false);
        }
    }
    return freeStack(&stack, stackAST, true);
}

