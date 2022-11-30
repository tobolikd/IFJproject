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
ht_table_t *fncTable; // extern missing?

// list->TokenArray[index]->type == t_string (jeho cislo v enumu) | takhle pristupuju k tokenum a jejich typum
// list->TokenArray[index]->data == Zadejte cislo pro vypocet faktorialu: | takhle k jejich datum

// <params> -> , <type> <var> <params> || eps
bool params(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        return true;
    }

    if (list->TokenArray[*index]->type == t_comma) // -> , [comma]
    {
        (*index)++;
        if (typeCheck(list, index) == false) // call type check for syntax check
        {
            return false;
        }
        (*index)++;
        if (list->TokenArray[*index]->type == t_varId)
        {
            (*index)++;
            if (params(list, index) == false) // -> <params>
            {
                return false;
            }
        }
        else
        {
            THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
            return false;
        }
    }
    else
    {
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        return false;
    }
    return true;
}

// <param> -> <type> <var> <params> || eps
bool param(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps (empty parameter list)
    {
        return true;
    }
    if (typeCheck(list, index) == false) // call type check for syntax check
    {
        return false;
    }
    (*index)++;
    if (list->TokenArray[*index]->type == t_varId)
    {
        (*index)++;
        if (params(list, index) == false) // -> <params>
        {
            return false;
        }
    }
    else
    {
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        return false;
    }
    return true;
}

// <type> -> int || string || float || ?int || ?string || ?float
bool typeCheck(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    switch (list->TokenArray[*index]->type)
    {
    case t_nullType:
        // debug_log("in NULL TYPE\n");
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
            THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
            return false;
        }
    case t_type:
        // debug_log("in TYPE\n");
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
            THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
            return false;
        }
    default:
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        return false;
    }
}

// <fnc-type> -> void || int || string || float || ?int || ?string || ?float
bool functionType(SYN_ANALYZER_TYPE_N_PARAM_PARAMS)
{
    if (!strcmp(list->TokenArray[*index]->data, "void"))
    {
        // debug_log("in void\n");
        if (list->TokenArray[*index]->type == t_type) // verify that void is correct Lexeme type
        {
            return true;
        }
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        return false;
    }
    else if (list->TokenArray[*index]->type == t_type || list->TokenArray[*index]->type == t_nullType)
    {
        // debug_log("in FNC TYPE\n");
        if (typeCheck(list, index) == false) // call type check
        {
            return false;
        }
        return true;
    }
    THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
    return false;
}

// <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
bool functionDeclare(SYN_ANALYZER_PARAMS)
{
    debug_log("FNC-DECL %i ", *index);
    if (list->TokenArray[*index]->type == t_function)
    {
        ht_table_t *fncDecTable = ht_init(); // create new symtable for Function Frame
        debug_log("FNC-DECL FUNCTION FUNCTION %i ", *index);
        (*index)++;
        if (list->TokenArray[*index]->type == t_functionId)
        {
            ht_item_t *curFunction = ht_search(fncTable, list->TokenArray[*index]->data); // find fuction declare by fuctionID in symtable, created in first run
            if (curFunction == NULL)
            {
                THROW_ERROR(INTERNAL_ERR, list->TokenArray[*index]->lineNum);
                return false;
            }
            stack_ast_push(stackSyn, ast_item_const(AST_FUNCTION_DECLARE, fnc_declare_data_const(curFunction, fncDecTable)));
            int counterParam = curFunction->fnc_data.paramCount;
            param_info_t *nextParam = curFunction->fnc_data.params;
            while (counterParam != 0) // insert params to symtable [Function Frame]
            {
                ht_insert(fncDecTable, nextParam->varId, nextParam->type, false);
                debug_log("VAR PARAM ID %s\n", nextParam->varId);
                counterParam--;
                nextParam = nextParam->next; // move to next parameter
            }
            (*index)++;
            if (list->TokenArray[*index]->type == t_lPar)
            {
                (*index)++;
                debug_log("FNC-DECL FUNCTION PARAM %i \n", *index);
                if (param(list, index) == false)
                {
                    return false;
                }
                if (list->TokenArray[*index]->type == t_rPar)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type == t_colon)
                    {
                        (*index)++;
                        if (functionType(list, index) == false)
                        {
                            return false;
                        }
                        (*index)++;
                        if (list->TokenArray[*index]->type == t_lCurl)
                        {
                            (*index)++;
                            if (list->TokenArray[*index]->type != t_rCurl)
                            {
                                if (statList(list, index, fncDecTable, stackSyn) == false)
                                {
                                    return false;
                                }
                            }
                            if (list->TokenArray[*index]->type == t_rCurl)
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
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
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
    if (list->TokenArray[*index]->type == t_rCurl) // end of statement with st-list or function declare - DON'T REMOVE!
    {
        return true;
    }
    if (statList(list, index, table, stackSyn) == false)
    {
        return false;
    }
    debug_log("ST-LIST %i ", *index);
    debug_log("Token type %s on line %d\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type], list->TokenArray[*index]->lineNum);
    return true;
}

// <stat> -> if || while || assign || return || eps
bool statement(SYN_ANALYZER_PARAMS)
{
    debug_log("STAT %i ", *index);
    switch (list->TokenArray[*index]->type)
    {
    case t_if: // if ( <expr> ) { <st-list> } else { <st-list> }
        stack_ast_push(stackSyn, ast_item_const(AST_IF, NULL));
        (*index)++;
        if (list->TokenArray[*index]->type == t_lPar)
        {
            (*index)++;
            if (list->TokenArray[*index]->type == t_rPar)
            {
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                return false;
            }
            if (parseExpression(list, index, table, stackSyn) == false)
            {
                debug_log("\nPREC FALSE: %i \n", errorCode);
                return false;
            }
            debug_log("\nPREC TRUE: %i \n", errorCode);
            if (list->TokenArray[*index]->type == t_rPar)
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_lCurl)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type != t_rCurl)
                    {
                        if (statList(list, index, table, stackSyn) == false)
                        {
                            return false;
                        }
                    }
                    if (list->TokenArray[*index]->type == t_rCurl)
                    {
                        stack_ast_push(stackSyn, ast_item_const(AST_END_BLOCK, NULL));
                        (*index)++;
                        if (list->TokenArray[*index]->type == t_else)
                        {
                            stack_ast_push(stackSyn, ast_item_const(AST_ELSE, NULL));
                            (*index)++;
                            if (list->TokenArray[*index]->type == t_lCurl)
                            {
                                (*index)++;
                                if (list->TokenArray[*index]->type != t_rCurl)
                                {
                                    if (statList(list, index, table, stackSyn) == false)
                                    {
                                        return false;
                                    }
                                }
                                if (list->TokenArray[*index]->type == t_rCurl)
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
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        return false;
    case t_while: // while ( <expr> ) { <st-list> }
        stack_ast_push(stackSyn, ast_item_const(AST_WHILE, NULL));
        (*index)++;
        if (list->TokenArray[*index]->type == t_lPar)
        {
            (*index)++;
            if (list->TokenArray[*index]->type == t_rPar)
            {
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                return false;
            }
            if (parseExpression(list, index, table, stackSyn) == false)
            {
                return false;
            }
            if (list->TokenArray[*index]->type == t_rPar)
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_lCurl)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type != t_rCurl)
                    {
                        if (statList(list, index, table, stackSyn) == false)
                        {
                            return false;
                        }
                    }
                    if (list->TokenArray[*index]->type == t_rCurl)
                    {
                        stack_ast_push(stackSyn, ast_item_const(AST_END_BLOCK, NULL));
                        return true;
                    }
                }
            }
        }
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        return false;
    case t_return: // return <expr> ;
        (*index)++;
        if (list->TokenArray[*index]->type == t_semicolon) // eps
        {
            stack_ast_push(stackSyn, ast_item_const(AST_RETURN_VOID, NULL));
            return true;
        }
        stack_ast_push(stackSyn, ast_item_const(AST_RETURN_EXPR, NULL));
        if (parseExpression(list, index, table, stackSyn) == false)
        {
            return false;
        }
        if (list->TokenArray[*index]->type == t_semicolon) // end <expr> with semicolon
        {
            return true;
        }
        else
        {
            THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
            return false;
        }
        break;
    default:
        if (list->TokenArray[*index]->type == t_varId) // <assign> -> <var> <r-side>
        {
            (*index)++;
            if (list->TokenArray[*index]->type == t_semicolon) // <r-side> -> eps
            {
                if (ht_search(table, list->TokenArray[(*index) - 1]->data) == NULL) // SEMANTIC CONTROL if variable was declerated
                {
                    THROW_ERROR(SEMANTIC_VARIABLE_ERR, list->TokenArray[*index]->lineNum);
                    return false;
                }
                // stack_ast_push(&stackSyn, ast_item_const(AST_VAR, ht_insert(table, list->TokenArray[(*index) - 1]->data, void_t, false)));
                return true;
            }
            else if (list->TokenArray[*index]->type != t_assign) // <r-side> -> <expr>
            {
                (*index)--;
                if (parseExpression(list, index, table, stackSyn) == false) // <assign> -> <expr>
                {
                    return false;
                }
                if (list->TokenArray[*index]->type == t_semicolon) // end <expr> with ; [semicolon]
                {
                    return true;
                }
            }
            else if (list->TokenArray[*index]->type == t_assign) // <r-side> -> = <expr>
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_semicolon) // <assign> -> <var> =; => ERROR
                {
                    THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                    return false;
                }
                ht_item_t *varItem = ht_insert(table, list->TokenArray[(*index) - 2]->data, void_t, false); // insert variable to symtable
                stack_ast_push(stackSyn, ast_item_const(AST_ASSIGN, NULL));
                stack_ast_push(stackSyn, ast_item_const(AST_VAR, varItem));
                if (parseExpression(list, index, table, stackSyn) == false)
                {
                    return false;
                }
                if (list->TokenArray[*index]->type == t_semicolon) // end <expr> with ; [semicolon]
                {
                    return true;
                }
            }
            else
            {
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                return false;
            }
        }
        else // <assign> -> <expr> || <stat> -> eps
        {
            debug_log("Token type %s on line %d\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type], list->TokenArray[*index]->lineNum);
            /* TODO - EPS STAT
            Teoreticky se to dá vyřešit, že se budu ptát, jestli tady nejsou nějaký klíčový tokeny a podle toho postupovat,
            třeba t_function mi to shodí return, protože se bude jednat o function declare */
            if (list->TokenArray[*index]->type == t_function) // <stat> -> eps
            {
                return true;
            }
            if (list->TokenArray[*index]->type == t_EOF) // <stat> -> eps
            {
                return true;
            }
            debug_log("Token type %s on line %d\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type], list->TokenArray[*index]->lineNum);
            if (parseExpression(list, index, table, stackSyn) == false) // <assign> -> <expr>
            {
                debug_log("FALSE Token type %s on line %d\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type], list->TokenArray[*index]->lineNum);
                return false;
            }
            else
            {
                debug_log("Token type %s on line %d\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type], list->TokenArray[*index]->lineNum);
                if (list->TokenArray[*index]->type == t_semicolon) // end <expr> with ; [semicolon]
                {
                    return true;
                }
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                return false;
            }

            (*index)++; // is this correct/necessary?
        }
        break;
    }
    return true;
}

// <seq-stats> -> <stat> <fnc-decl> <seq-stats> || eps
bool seqStats(SYN_ANALYZER_PARAMS)
{
    // debug_log("SEQ-STAT %i ", *index);
    bool end;
    end = statement(list, index, table, stackSyn);
    if ((*index) == list->length || end == false || list->TokenArray[*index]->type == t_EOF)
    {
        debug_log("End of program\n");
        return true;
    }
    end = functionDeclare(list, index, table, stackSyn);
    if ((*index) == list->length || end == false || list->TokenArray[*index]->type == t_EOF)
    {
        debug_log("End of program\n");
        return true;
    }
    (*index)++;
    debug_log("\nLIST LENGHT: %d\n", list->length);
    if ((*index) == list->length || end == false || list->TokenArray[*index]->type == t_EOF)
    {
        debug_log("End of program\n");
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
    debug_log("PROGRAM %i\n", *index);
    if (seqStats(list, index, table, stackSyn) == false)
    {
        return false;
    }
    return true;
}

void SyntaxDtor(ht_table_t *table, stack_ast_t *stackAST)
{
    ht_delete_all(table);
    while (!stack_ast_empty(stackAST))
    {
        stack_ast_pop(stackAST);
    }

    while (!stack_declare_empty(&stackDeclare))
    {
        stack_declare_pop(&stackDeclare);
    }

    free(stackAST);
}

SyntaxItem SyntaxItemCtor(ht_table_t *table, stack_ast_t *stackAST, bool correct)
{
    SyntaxItem SyntaxItem;
    SyntaxItem.table = table;
    SyntaxItem.stackAST = stackAST;
    SyntaxItem.correct = correct;
    return SyntaxItem;
}

SyntaxItem synAnalyser(TokenList *list)
{
    int index = 0;
    ht_table_t *table = ht_init();
    stack_ast_t *stackSyn = (stack_ast_t *)malloc(sizeof(stack_ast_t));
    if (stackSyn == NULL)
    {
        MALLOC_ERR;
    }
    stack_ast_init(stackSyn);
    stack_declare_init(&stackDeclare);  // initialize stack for Function Frames

    /* RECURSIVE DESCENT */
    if (checkSyntax(list, &index, table, stackSyn) == false)
    {
        SyntaxDtor(table, stackSyn);
        return SyntaxItemCtor(NULL, NULL, false);
    }

    return SyntaxItemCtor(table, stackSyn, true);
}
