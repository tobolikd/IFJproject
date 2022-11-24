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
ht_table_t *GlobalTable;
stack_ast_t stackSyn;

// list->TokenArray[index]->type == t_string (jeho cislo v enumu) | takhle pristupuju k tokenum a jejich typum
// list->TokenArray[index]->data == Zadejte cislo pro vypocet faktorialu: | takhle k jejich datum

// <params> -> , <type> <var> <params> || eps
bool params(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        return true;
    }

    if (list->TokenArray[*index]->type == t_comma) // -> , [comma]
    {
        (*index)++;
        if (typeCheck(list, index) == false) // call type check
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
bool param(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        return true;
    }
    if (typeCheck(list, index) == false) // call type check
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
bool typeCheck(TokenList *list, int *index)
{
    switch (list->TokenArray[*index]->type)
    {
    case t_nullType:
        debug_log("in NULL TYPE\n");
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
        debug_log("in TYPE\n");
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
bool functionType(TokenList *list, int *index)
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
bool functionDeclare(TokenList *list, int *index, ht_table_t *table)
{
    debug_log("FNC-DECL %i ", *index);
    if (list->TokenArray[*index]->type == t_function)
    {
        // ht_table_t *fntable = ht_init();    // create new symtable for Local Frame
        debug_log("FNC-DECL FUNCTION FUNCTION %i ", *index);
        (*index)++;
        if (list->TokenArray[*index]->type == t_functionId)
        {
            // ht_search(table, list->TokenArray[*index]->type); // USE WITH GLOBAL VAR FROM ADA'S FIRST
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
                                if (statList(list, index, table) == false)
                                {
                                    return false;
                                }
                            }
                            if (list->TokenArray[*index]->type == t_rCurl)
                            {
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
bool statList(TokenList *list, int *index, ht_table_t *table)
{
    if (statement(list, index, table) == false)
    {
        return false;
    }
    (*index)++;
    if (list->TokenArray[*index]->type == t_rCurl) // end of statement with st-list or function declare - DON'T REMOVE!
    {
        return true;
    }
    if (statList(list, index, table) == false)
    {
        return false;
    }
    debug_log("ST-LIST %i ", *index);
    debug_log("%s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
    return true;
}

// <stat> -> if || while || assign || return || eps
bool statement(TokenList *list, int *index, ht_table_t *table)
{
    debug_log("STAT %i ", *index);
    switch (list->TokenArray[*index]->type)
    {
    case t_if: // if ( <expr> ) { <st-list> } else { <st-list> }
        (*index)++;
        if (list->TokenArray[*index]->type == t_lPar)
        {
            (*index)++;
            if (parseExpression(list, index, table, &stackSyn) == false)
            {
                return false;
            }
            //(*index)++;
            if (list->TokenArray[*index]->type == t_rPar)
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_lCurl)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type != t_rCurl)
                    {
                        if (statList(list, index, table) == false)
                        {
                            return false;
                        }
                    }
                    if (list->TokenArray[*index]->type == t_rCurl)
                    {
                        (*index)++;
                        if (list->TokenArray[*index]->type == t_else)
                        {
                            (*index)++;
                            if (list->TokenArray[*index]->type == t_lCurl)
                            {
                                (*index)++;
                                if (list->TokenArray[*index]->type != t_rCurl)
                                {
                                    if (statList(list, index, table) == false)
                                    {
                                        return false;
                                    }
                                }
                                if (list->TokenArray[*index]->type == t_rCurl)
                                {
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
        break;
    case t_while: // while ( <expr> ) { <st-list> }
        (*index)++;
        if (list->TokenArray[*index]->type == t_lPar)
        {
            (*index)++;
            if (parseExpression(list, index, table, &stackSyn) == false)
            {
                return false;
            }
            //(*index)++;
            if (list->TokenArray[*index]->type == t_rPar)
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_lCurl)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type != t_rCurl)
                    {
                        if (statList(list, index, table) == false)
                        {
                            return false;
                        }
                    }
                    if (list->TokenArray[*index]->type == t_rCurl)
                    {
                        return true;
                    }
                }
            }
            THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
            return false;
        }
        break;
    case t_return: // return <expr> ;
        (*index)++;
        // if (list->TokenArray[*index]->type == t_semicolon)
        // {
        //     return true;
        // }
        if (parseExpression(list, index, table, &stackSyn) == false)
        {
            return false;
        }
        if (list->TokenArray[*index]->type == t_semicolon)
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
        if (list->TokenArray[*index]->type == t_semicolon) // <assign> -> eps
        {
            return true;
        }
        else if (list->TokenArray[*index]->type == t_varId) // <assign> -> <var> <r-side>
        {
            // BETTER? stack_ast_push(stack, ht_insert(table, list->TokenArray[*index]->data, void_t, false));
            (*index)++;
            if (list->TokenArray[*index]->type == t_semicolon) // <r-side> -> eps
            {
                if (ht_search(table, list->TokenArray[(*index) - 1]->data) == NULL) // SEMANTIC CONTROL if variable was declerated
                {
                    THROW_ERROR(SEMANTIC_VARIABLE_ERR, list->TokenArray[*index]->lineNum);
                    return false;
                }
                return true;
            }
            else if (list->TokenArray[*index]->type == t_assign) // <r-side> -> = <expr>
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_semicolon) // <assign> -> <var> =; => ERROR
                {
                    THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                    return false;
                }
                ht_item_t *item = ht_insert(table, list->TokenArray[(*index) - 2]->data, void_t, false); // insert variable to symtable
                // while (list->TokenArray[*index]->type != t_semicolon)                                    // temporary solution, before precedent Analyser is done
                // {
                //     (*index)++;
                // }
                if (parseExpression(list, index, table, &stackSyn) == false)
                {
                    return false;
                }
                if (list->TokenArray[*index]->type == t_semicolon) // end <expr> with ; [semicolon]
                {
                    return true;
                }
                // FIX: $a = 5; se momentálně nevyhodnotí jako chyba, protože tu není žádný else,
                // předpokládám, že se o tohle postará precendAnalyser
                // zbytek programu je pak posunutý
            }
            else
            {
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                return false;
            }
        }
        else // <assign> -> <expr> || <stat> -> eps
        {
            /* TODO - EPS STAT
            Teoreticky se to dá vyřešit, že se budu ptát, jestli tady nejsou nějaký klíčový tokeny a podle toho postupovat,
            třeba t_function mi to shodí return, protože se bude jednat o function declare */

            (*index)++;
            if (parseExpression(list, index, table, &stackSyn) == false)
            {
                return false;
            }
        }
        break;
    }
    return true;
}

// <seq-stats> -> <stat> <fnc-decl> <seq-stats> || eps
bool seqStats(TokenList *list, int *index, ht_table_t *table)
{
    // debug_log("SEQ-STAT %i ", *index);
    bool end;
    end = statement(list, index, table);
    if ((*index) == list->length || end == false || list->TokenArray[*index]->type == t_EOF)
    {
        debug_log("End of program\n");
        return true;
    }
    end = functionDeclare(list, index, table);
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
    if (seqStats(list, index, table) == false)
    {
        return false;
    }
    return true;
}

// <prog> -> <prolog> <seq-stats> <epilog>
bool checkSyntax(TokenList *list, int *index, ht_table_t *table)
{
    debug_log("PROGRAM %i\n", *index);
    if (seqStats(list, index, table) == false)
    {
        return false;
    }
    return true;
}

bool synAnalyser(TokenList *list)
{
    int index = 0;
    ht_table_t *table = ht_init();
    GlobalTable = ht_init();
    // stack_ast_t stackSyn;
    stack_ast_init(&stackSyn);
    // if (!list->TokenArray[0]) // what does this do?
    // {
    //     errorCode = LEXICAL_ERR;
    //     return false;
    // }
    /* START OF RECURSIVE DESCENT */
    if (checkSyntax(list, &index, table) == false)
    {
        ht_delete_all(table);
        while (!stack_ast_empty(&stackSyn))
        {
            stack_ast_pop(&stackSyn);
        }
        return false;
    }
    ht_delete_all(table);
    while (!stack_ast_empty(&stackSyn))
    {
        stack_ast_pop(&stackSyn);
    }
    return true;
}
