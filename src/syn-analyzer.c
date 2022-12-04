/**
 * @file syn-analyzer.c
 * @author Jakub Mikysek (xmikys03)
 * @brief Syntatic Analyser for interpreter IFJcode22
 */

#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "symtable.h"
#include "error-codes.h"
#include "ast.h"
#include "stack.h"
#include "preced-analyzer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum ifjErrCode errorCode;
ht_table_t *fncTable;
stack_declare_t stackDeclare;

bool params(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    if (TOKEN_TYPE == t_rPar) // -> eps
    {
        return true;
    }

    if (TOKEN_TYPE == t_comma) // -> , [comma]
    {
        (*index)++;
        if (typeCheck(list, index) == false) // call type check for syntax check
        {
            return false;
        }
        (*index)++;
        if (TOKEN_TYPE == t_varId)
        {
            (*index)++;
            if (params(list, index) == false) // -> <params>
            {
                return false;
            }
        }
        else
        {
            THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
            return false;
        }
    }
    else
    {
        THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
        return false;
    }
    return true;
}

bool param(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    if (TOKEN_TYPE == t_rPar) // -> eps (empty parameter list)
    {
        return true;
    }
    if (typeCheck(list, index) == false) // call type check for syntax check
    {
        return false;
    }
    (*index)++;
    if (TOKEN_TYPE == t_varId)
    {
        (*index)++;
        if (params(list, index) == false) // -> <params>
        {
            return false;
        }
    }
    else
    {
        THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
        return false;
    }
    return true;
}

bool checkType(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    if (!strcmp(list->TokenArray[*index]->data, "int"))
    {
        return true;
    }
    else if (!strcmp(list->TokenArray[*index]->data, "float"))
    {
        return true;
    }
    else if (!strcmp(list->TokenArray[*index]->data, "string"))
    {
        return true;
    }
    else
    {
        THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
        return false;
    }
}

bool typeCheck(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    switch (TOKEN_TYPE)
    {
    case t_nullType:
        return checkType(list, index);
    case t_type:
        return checkType(list, index);
    default:
        THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
        return false;
    }
}

bool functionType(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    if (!strcmp(list->TokenArray[*index]->data, "void"))
    {
        if (TOKEN_TYPE == t_type) // verify that void is correct Lexeme type
        {
            return true;
        }
        THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
        return false;
    }
    else if (TOKEN_TYPE == t_type || TOKEN_TYPE == t_nullType)
    {
        return typeCheck(list, index); // call type check
    }
    THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
    return false;
}

// <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
bool functionDeclare(TokenList *list, int *index, stack_ast_t *stackSyn)
{
    if (TOKEN_TYPE == t_function)
    {
        ht_table_t *fncDecTable = ht_init(); // create new symtable for Function Frame
        (*index)++;
        if (TOKEN_TYPE == t_functionId)
        {
            ht_item_t *curFunction = ht_search(fncTable, list->TokenArray[*index]->data); // find fuction declare by fuctionID in symtable, created in first run
            if (curFunction == NULL)
            {
                THROW_ERROR(INTERNAL_ERR, TOKEN_LINE_NUM);
                return false;
            }
            stack_ast_push(stackSyn, ast_item_const(AST_FUNCTION_DECLARE, fnc_declare_data_const(curFunction, fncDecTable)));
            int counterParam = curFunction->fnc_data.paramCount;
            param_info_t *nextParam = curFunction->fnc_data.params;
            while (counterParam != 0) // insert params to symtable [Function Frame]
            {
                debug_log("Inserting parameter %s to symtable\n", nextParam->varId);
                ht_insert(fncDecTable, nextParam->varId, nextParam->type, false);
                counterParam--;
                nextParam = nextParam->next; // move to next parameter
            }
            (*index)++;
            if (TOKEN_TYPE == t_lPar)
            {
                (*index)++;
                if (param(list, index) == false)
                {
                    return false;
                }
                if (TOKEN_TYPE == t_rPar)
                {
                    (*index)++;
                    if (TOKEN_TYPE == t_colon)
                    {
                        (*index)++;
                        if (functionType(list, index) == false)
                        {
                            return false;
                        }
                        (*index)++;
                        if (TOKEN_TYPE == t_lCurl)
                        {
                            (*index)++;
                            if (TOKEN_TYPE != t_rCurl)
                            {
                                if (statList(list, index, fncDecTable, stackSyn) == false)
                                {
                                    return false;
                                }
                            }
                            if (TOKEN_TYPE == t_rCurl)
                            {
                                stack_declare_push(&stackDeclare, fncDecTable);
                                stack_ast_push(stackSyn, ast_item_const(AST_END_BLOCK, NULL));
                                return true;
                            }
                        }
                    }
                }
            }
        }
        THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
        return false;
    }
    return true; // -> eps
}

// <st-list> -> <stat> <st-list> || eps
bool statList(SYN_ANALYZER_PARAMS)
{
    if (statement(list, index, table, stackSyn) == false)
    {
        return false;
    }
    (*index)++;
    if (TOKEN_TYPE == t_rCurl) // end of statement with st-list or function declare - DON'T REMOVE!
    {
        return true;
    }
    if (statList(list, index, table, stackSyn) == false)
    {
        return false;
    }
    return true;
}

bool statementIf(SYN_ANALYZER_PARAMS)
{
    stack_ast_push(stackSyn, ast_item_const(AST_IF, NULL));
    (*index)++;
    if (TOKEN_TYPE == t_lPar)
    {
        (*index)++;
        if (TOKEN_TYPE != t_rPar)
        {
            if (parseExpression(list, index, table, stackSyn) == false)
            {
                return false;
            }
            if (TOKEN_TYPE == t_rPar)
            {
                (*index)++;
                if (TOKEN_TYPE == t_lCurl)
                {
                    (*index)++;
                    if (TOKEN_TYPE != t_rCurl)
                    {
                        if (statList(list, index, table, stackSyn) == false)
                        {
                            return false;
                        }
                    }
                    if (TOKEN_TYPE == t_rCurl)
                    {
                        stack_ast_push(stackSyn, ast_item_const(AST_END_BLOCK, NULL));
                        (*index)++;
                        if (TOKEN_TYPE == t_else)
                        {
                            stack_ast_push(stackSyn, ast_item_const(AST_ELSE, NULL));
                            (*index)++;
                            if (TOKEN_TYPE == t_lCurl)
                            {
                                (*index)++;
                                if (TOKEN_TYPE != t_rCurl)
                                {
                                    if (statList(list, index, table, stackSyn) == false)
                                    {
                                        return false;
                                    }
                                }
                                if (TOKEN_TYPE == t_rCurl)
                                {
                                    stack_ast_push(stackSyn, ast_item_const(AST_END_BLOCK, NULL));
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
    return false;
}

bool statementWhile(SYN_ANALYZER_PARAMS)
{
    stack_ast_push(stackSyn, ast_item_const(AST_WHILE, NULL));
    (*index)++;
    if (TOKEN_TYPE == t_lPar)
    {
        (*index)++;
        if (TOKEN_TYPE != t_rPar)
        {
            if (parseExpression(list, index, table, stackSyn) == false)
            {
                return false;
            }
            if (TOKEN_TYPE == t_rPar)
            {
                (*index)++;
                if (TOKEN_TYPE == t_lCurl)
                {
                    (*index)++;
                    if (TOKEN_TYPE != t_rCurl)
                    {
                        if (statList(list, index, table, stackSyn) == false)
                        {
                            return false;
                        }
                    }
                    if (TOKEN_TYPE == t_rCurl)
                    {
                        stack_ast_push(stackSyn, ast_item_const(AST_END_BLOCK, NULL));
                        return true;
                    }
                }
            }
        }
    }
    THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
    return false;
}

bool statementReturn(SYN_ANALYZER_PARAMS)
{
    (*index)++;
    if (TOKEN_TYPE == t_semicolon) // <stat> -> return;
    {
        stack_ast_push(stackSyn, ast_item_const(AST_RETURN_VOID, NULL));
        return true;
    }
    stack_ast_push(stackSyn, ast_item_const(AST_RETURN_EXPR, NULL));
    if (parseExpression(list, index, table, stackSyn) == false)
    {
        return false;
    }
    if (TOKEN_TYPE == t_semicolon) // end <expr> with semicolon
    {
        return true;
    }
    THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
    return false;
}

bool statementVariable(SYN_ANALYZER_PARAMS)
{
    (*index)++;
    if (TOKEN_TYPE == t_semicolon) // <r-side> -> eps
    {
        if (ht_search(table, list->TokenArray[(*index) - 1]->data) == NULL) // SEMANTIC CONTROL if variable was declerated
        {
            THROW_ERROR(SEMANTIC_VARIABLE_ERR, TOKEN_LINE_NUM);
            return false;
        }
        return true;
    }
    else if (TOKEN_TYPE != t_assign) // <r-side> -> <expr>
    {
        (*index)--;
        if (parseExpression(list, index, table, stackSyn) == false) // <assign> -> <expr>
        {
            return false;
        }
        if (TOKEN_TYPE == t_semicolon) // end <expr> with ; [semicolon]
        {
            return true;
        }
        THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
        return false;
    }
    else if (TOKEN_TYPE == t_assign) // <r-side> -> = <expr>
    {
        (*index)++;
        if (TOKEN_TYPE != t_semicolon) // <assign> -> <var> =; => ERROR
        {
            ht_item_t *varItem = ht_insert(table, list->TokenArray[(*index) - 2]->data, void_t, false); // insert variable to symtable
            stack_ast_push(stackSyn, ast_item_const(AST_ASSIGN, NULL));
            stack_ast_push(stackSyn, ast_item_const(AST_VAR, varItem));
            if (parseExpression(list, index, table, stackSyn) == false)
            {
                return false;
            }
            if (TOKEN_TYPE == t_semicolon) // end <expr> with ; [semicolon]
            {
                return true;
            }
        }
    }
    THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
    return false;
}

bool statement(SYN_ANALYZER_PARAMS)
{
    switch (TOKEN_TYPE)
    {
    case t_if: // if ( <expr> ) { <st-list> } else { <st-list> }
        return statementIf(list, index, table, stackSyn);
    case t_while: // while ( <expr> ) { <st-list> }
        return statementWhile(list, index, table, stackSyn);
    case t_return: // return <expr> ;
        return statementReturn(list, index, table, stackSyn);
    case t_varId: // <assign> -> <var> <r-side>
        return statementVariable(list, index, table, stackSyn);
    default:
        // <assign> -> <expr> || <stat> -> eps
        if (TOKEN_TYPE == t_function) // <stat> -> eps
        {
            return true;
        }
        if (TOKEN_TYPE == t_EOF) // <stat> -> eps
        {
            return true;
        }

        if (parseExpression(list, index, table, stackSyn) == false) // <assign> -> <expr>
        {
            return false;
        }
        else
        {
            if (TOKEN_TYPE == t_semicolon) // end <expr> with ; [semicolon]
            {
                return true;
            }
            THROW_ERROR(SYNTAX_ERR, TOKEN_LINE_NUM);
            return false;
        }

        (*index)++;
        break;
    }
    return true;
}

// <seq-stats> -> <stat> <fnc-decl> <seq-stats> || eps
bool seqStats(SYN_ANALYZER_PARAMS)
{
    bool programContinue; // if program return false, exit to propagate Error
    programContinue = statement(list, index, table, stackSyn);
    CHECK_END_PROGRAM();
    programContinue = functionDeclare(list, index, stackSyn);
    CHECK_END_PROGRAM();
    (*index)++;
    if ((*index) == list->length || TOKEN_TYPE == t_EOF)
    {
        return true;
    }
    if (seqStats(list, index, table, stackSyn) == false)
    {
        return false;
    }
    return true;
}

// <prog> -> <prolog> <seq-stats> <epilog>
bool checkSyntax(SYN_ANALYZER_PARAMS)
{
    if (seqStats(list, index, table, stackSyn) == false)
    {
        return false;
    }
    return true;
}

void SyntaxDtor(SyntaxItem *SyntaxItem)
{
    ht_delete_all(SyntaxItem->table);
    while (!stack_ast_empty(SyntaxItem->stackAST))
    {
        stack_ast_pop(SyntaxItem->stackAST);
    }

    free(SyntaxItem->stackAST);
    free(SyntaxItem);
}

SyntaxItem *SyntaxItemCtor(ht_table_t *table, stack_ast_t *stackAST, bool correct)
{
    SyntaxItem *syntaxItem = (SyntaxItem *)malloc(sizeof(SyntaxItem));
    CHECK_MALLOC_PTR(syntaxItem);
    syntaxItem->table = table;
    syntaxItem->stackAST = stackAST;
    syntaxItem->correct = correct;
    return syntaxItem;
}

SyntaxItem *synAnalyser(TokenList *list)
{
    int index = 0;
    ht_table_t *table = ht_init();
    stack_ast_t *stackSyn = (stack_ast_t *)malloc(sizeof(stack_ast_t));
    if (stackSyn == NULL)
    {
        MALLOC_ERR;
        return SyntaxItemCtor(table, stackSyn, false);
    }
    stack_ast_init(stackSyn);
    stack_declare_init(&stackDeclare); // initialize stack for Function Frames

    /* RECURSIVE DESCENT */
    if (checkSyntax(list, &index, table, stackSyn) == false)
    {
        return SyntaxItemCtor(table, stackSyn, false);
    }

    return SyntaxItemCtor(table, stackSyn, true);
}
