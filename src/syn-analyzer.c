/**
 * @file syn-analyzer.c
 * @author Jakub Mikysek (xmikys03)
 * @brief Syntatic Analyser for interpreter IFJcode22
 */

#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "symtable.h"
#include "error-codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// list->TokenArray[index]->type == t_string (jeho cislo v enumu) | takhle pristupuju k tokenum a jejich typum
// list->TokenArray[index]->data == Zadejte cislo pro vypocet faktorialu: | takhle k jejich datum

// <params> -> , <type> <var> <params> || eps
void params(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        return;
    }

    if (list->TokenArray[*index]->type == t_comma) // -> , [comma]
    {
        (*index)++;
        type(list, index);
        (*index)++;
        if (list->TokenArray[*index]->type == t_varId)
        {
            (*index)++;
            params(list, index);
        }
        else
        {
            errorCode = SYNTAX_ERR;
        }
    }
    else
    {
        errorCode = SYNTAX_ERR;
    }
}

// <param> -> <type> <var> <params> || eps
void param(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        return;
    }
    type(list, index);
    (*index)++;
    if (list->TokenArray[*index]->type == t_varId)
    {
        (*index)++;
        params(list, index);
    }
    else
    {
        errorCode = SYNTAX_ERR;
    }
}

// <type> -> int || string || float || ?int || ?string || ?float
void type(TokenList *list, int *index)
{
    switch (list->TokenArray[*index]->type)
    {
    case t_nullType:
        // debug_log("in NULL TYPE\n");
        if (!strcmp(list->TokenArray[*index]->data, "int"))
        {
            return;
        }
        else if (!strcmp(list->TokenArray[*index]->data, "float"))
        {
            return;
        }
        else if (!strcmp(list->TokenArray[*index]->data, "string"))
        {
            return;
        }
        else
        {
            errorCode = SYNTAX_ERR;
            return;
        }
    case t_type:
        // debug_log("in TYPE\n");
        if (!strcmp(list->TokenArray[*index]->data, "int"))
        {
            return;
        }
        else if (!strcmp(list->TokenArray[*index]->data, "float"))
        {
            return;
        }
        else if (!strcmp(list->TokenArray[*index]->data, "string"))
        {
            return;
        }
        else
        {
            errorCode = SYNTAX_ERR;
            return;
        }
    default:
        errorCode = SYNTAX_ERR;
        return;
    }
}

// <fnc-type> -> void || int || string || float || ?int || ?string || ?float
void functionType(TokenList *list, int *index)
{
    if (!strcmp(list->TokenArray[*index]->data, "void"))
    {
        // debug_log("in void\n");
        if (list->TokenArray[*index]->type == t_type)
        {
            return;
        }
        errorCode = SYNTAX_ERR;
    }
    else if (list->TokenArray[*index]->type == t_type || list->TokenArray[*index]->type == t_nullType)
    {
        // debug_log("in FNC TYPE\n");
        type(list, index);
        return;
    }
    errorCode = SYNTAX_ERR;
}

// <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
void functionDeclare(TokenList *list, int *index)
{
    debug_log("FNC-DECL %i ", *index);
    if (list->TokenArray[*index]->type == t_function)
    {
        debug_log("FNC-DECL FUNCTION FUNCTION %i ", *index);
        (*index)++;
        if (list->TokenArray[*index]->type == t_functionId)
        {
            (*index)++;
            if (list->TokenArray[*index]->type == t_lPar)
            {
                (*index)++;
                param(list, index);
                if (list->TokenArray[*index]->type == t_rPar)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type == t_colon)
                    {
                        (*index)++;
                        functionType(list, index);
                        (*index)++;
                        if (list->TokenArray[*index]->type == t_lCurl)
                        {
                            (*index)++;
                            if (list->TokenArray[*index]->type != t_rCurl)
                            {
                                statList(list, index);
                            }
                            if (list->TokenArray[*index]->type == t_rCurl)
                            {
                                return;
                            }
                        }
                    }
                }
            }
        }
        errorCode = SYNTAX_ERR; // is this correct?
        return;
    }
    return;
}

// <st-list> -> <stat> <st-list> || eps
void statList(TokenList *list, int *index)
{
    statement(list, index);
    (*index)++;
    if (list->TokenArray[*index]->type == t_rCurl)
    {
        return;
    }
    statList(list, index);
    debug_log("ST-LIST %i ", *index);
    debug_log("%s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
    return;
}

// <stat> -> if || while || assign || return || eps
void statement(TokenList *list, int *index)
{
    debug_log("STAT %i ", *index);
    switch (list->TokenArray[*index]->type)
    {
    case t_if: // if ( <expr> ) { <st-list> } else { <st-list> }
        (*index)++;
        if (list->TokenArray[*index]->type == t_lPar)
        {
            // (*index)++;
            // precendAnalyser();
            (*index)++;
            if (list->TokenArray[*index]->type == t_rPar)
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_lCurl)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type != t_rCurl)
                    {
                        statList(list, index);
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
                                    statList(list, index);
                                }
                                if (list->TokenArray[*index]->type == t_rCurl)
                                {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
            errorCode = SYNTAX_ERR;
            return;
        }
        return;
    case t_while: // while ( <expr> ) { <st-list> }
        (*index)++;
        if (list->TokenArray[*index]->type == t_lPar)
        {
            // (*index)++;
            // precendAnalyser();
            (*index)++;
            if (list->TokenArray[*index]->type == t_rPar)
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_lCurl)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type != t_rCurl)
                    {
                        statList(list, index);
                    }
                    if (list->TokenArray[*index]->type == t_rCurl)
                    {
                        return;
                    }
                }
            }
            errorCode = SYNTAX_ERR;
            return;
        }
        return;
    case t_return: // return <expr> ;
        (*index)++;
        // precendAnalyser();
        if (list->TokenArray[*index]->type == t_semicolon)
        {
            return;
        }
        else
        {
            errorCode = SYNTAX_ERR;
            return;
        }
        return;
    default:
        if (list->TokenArray[*index]->type == t_semicolon) // <assign> -> eps
        {
            return;
        }
        else if (list->TokenArray[*index]->type == t_varId) // <assign> -> <var> <r-side>
        {
            debug_log("\nINDEX: %s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
            (*index)++;
            debug_log("\nINDEX: %s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
            if (list->TokenArray[*index]->type == t_semicolon) // <r-side> -> eps
            {
                return;
            }
            else if (list->TokenArray[*index]->type == t_assign) // <r-side> -> = <expr>
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_semicolon) // <assign> -> <var> =; => ERROR
                {
                    errorCode = SYNTAX_ERR;
                    return;
                }
                if (list->TokenArray[*index]->type == t_functionId) // <assign> -> <var> = functionId ( <param> ) / Function Call
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type == t_lPar)
                    {
                        (*index)++;
                        param(list, index);
                        if (list->TokenArray[*index]->type == t_rPar)
                        {
                            (*index)++;
                            if (list->TokenArray[*index]->type == t_semicolon) // end Function Call with ; [semicolon]
                            {
                                return;
                            }
                        }
                    }
                    errorCode = SYNTAX_ERR;
                    return;
                }
                while (list->TokenArray[*index]->type != t_semicolon) // temporary solution, before precedent Analyser is done
                {
                    (*index)++;
                }
                // precendAnalyser();
                if (list->TokenArray[*index]->type == t_semicolon) // end <expr> with ; [semicolon]
                {
                    return;
                }
                // FIX: $a = 5; se momentálně nevyhodnotí jako chyba, protože tu není žádný else,
                // předpokládám, že se o tohle postará precendAnalyser
                // zbytek programu je pak posunutý
            }
            else
            {
                errorCode = SYNTAX_ERR;
                return;
            }
        }
        else if (list->TokenArray[*index]->type == t_functionId) // <assign> -> functionId ( <param> ) / Function Call
        {
            (*index)++;
            if (list->TokenArray[*index]->type == t_lPar)
            {
                (*index)++;
                param(list, index);
                if (list->TokenArray[*index]->type == t_rPar)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type == t_semicolon) // end Function Call with ; [semicolon]
                    {
                        return;
                    }
                }
            }
            errorCode = SYNTAX_ERR;
            return;
        }
        else // <assign> -> <expr> || <stat> -> eps
        {
            /* TODO - EPS STAT */

            // (*index)++;
            // precendAnalyser();
        }
        return;
    }
    return;
}

// <seq-stats> -> <stat> <fnc-decl> <seq-stats> || eps
void seqStats(TokenList *list, int *index)
{
    // debug_log("SEQ-STAT %i ", *index);
    statement(list, index);
    functionDeclare(list, index);
    (*index)++;
    debug_log("\nLIST LENGHT: %d\n", list->length);
    if ((*index) == list->length)
    {
        debug_log("End of program\n");
        return;
    }
    seqStats(list, index);
    return;
}

// <prog> -> <prolog> <seq-stats> <epilog>
void checkSyntax(TokenList *list, int *index)
{
    debug_log("PROGRAM %i\n", *index);
    seqStats(list, index);
    return;
}

void synAnalyser(TokenList *list)
{
    int index = 0;
    if (!list->TokenArray[0])
    {
        errorCode = SYNTAX_ERR;
        return;
    }
    /* START OF RECURSIVE DESCENT */
    checkSyntax(list, &index);
    return;
}
