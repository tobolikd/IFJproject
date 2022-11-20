#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "symtable.h"
#include "error-codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TOKEN_TYPE_STRING[list->TokenArray[index]->type] == t_string | takhle pristupuju k tokenum a jejich typum
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
        // debug_print("in NULL TYPE\n");
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
        // debug_print("in TYPE\n");
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
        // debug_print("in void\n");
        if (list->TokenArray[*index]->type == t_type)
        {
            return;
        }
        errorCode = SYNTAX_ERR;
    }
    else if (list->TokenArray[*index]->type == t_type || list->TokenArray[*index]->type == t_nullType)
    {
        // debug_print("in FNC TYPE\n");
        type(list, index);
        return;
    }
    errorCode = SYNTAX_ERR;
}

// <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
void functionDeclare(TokenList *list, int *index)
{
    debug_print("FNC-DECL %i ", *index);
    if (list->TokenArray[*index]->type == t_function)
    {
        debug_print("FNC-DECL FUNCTION FUNCTION %i ", *index);
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
    stat(list, index);
    (*index)++;
    if (list->TokenArray[*index]->type == t_rCurl)
    {
        return;
    }
    statList(list, index);
    debug_print("ST-LIST %i ", *index);
    debug_print("%s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
    return;
}

// <stat> -> if || while || assign || return || eps
void stat(TokenList *list, int *index)
{
    debug_print("STAT %i ", *index);
    switch (list->TokenArray[*index]->type)
    {
    case t_if:  // if ( <expr> ) { <st-list> } else { <st-list> }
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
        }
    case t_while:   // while ( <expr> ) { <st-list> }
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
        }
    case t_return:  // return <expr> ;
        (*index)++;
        // precendAnalyser();
        if (list->TokenArray[*index]->type == t_semicolon)
        {
            return;
        }
        else
        {
            errorCode = SYNTAX_ERR;
        }
    default:
        return;
    }
    return;
}

// <seq-stats> -> <stat> <fnc-decl> <seq-stats> || eps
void seqStats(TokenList *list, int *index)
{
    // debug_print("SEQ-STAT %i ", *index);
    stat(list, index);
    functionDeclare(list, index);
    (*index)++;
    debug_print("\nLIST LENGHT: %d\n", list->length);
    if ((*index) == list->length)
    {
        debug_print("End of program\n");
        return;
    }
    seqStats(list, index);
    return;
    // printf("%s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
}

// <prog> -> <prolog> <seq-stats> <epilog>
void checkSyntax(TokenList *list, int *index)
{
    debug_print("PROGRAM %i\n", *index);
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
