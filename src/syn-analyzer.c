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

enum ifjErrCode errorCode;

// list->TokenArray[index]->type == t_string (jeho cislo v enumu) | takhle pristupuju k tokenum a jejich typum
// list->TokenArray[index]->data == Zadejte cislo pro vypocet faktorialu: | takhle k jejich datum

// <call-params> -> , <var> <call-params> || , <literal> <call-params> || eps
bool callParams(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        return true;
    }

    if (list->TokenArray[*index]->type == t_comma) // -> , [comma]
    {
        (*index)++;
        /* <literal> */
        if (list->TokenArray[*index]->type == t_int) // int - e.g. 5
        {
            (*index)++;
            if (callParams(list, index) == false)
            {
                return false;
            }
        }
        else if (list->TokenArray[*index]->type == t_float) // float - e.g. 0.5
        {
            (*index)++;
            if (callParams(list, index) == false)
            {
                return false;
            }
        }
        else if (list->TokenArray[*index]->type == t_string) // string - e.g. hello world!
        {
            (*index)++;
            if (callParams(list, index) == false)
            {
                return false;
            }
        }
        /* <var> */
        else if (list->TokenArray[*index]->type == t_varId) // variable - e.g. $id
        {
            (*index)++;
            if (callParams(list, index) == false)
            {
                return false;
            }
        }
        else
        {
            errorCode = SYNTAX_ERR;
            return false;
        }
    }
    else
    {
        errorCode = SYNTAX_ERR;
        return false;
    }
    return true;
}

// <call-param> -> <var> <call-params> || <literal> <call-params> || eps
bool callParam(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        return true;
    }
    /* <literal> */
    else if (list->TokenArray[*index]->type == t_int) // int - e.g. 5
    {
        (*index)++;
        if (callParams(list, index) == false)
        {
            return false;
        }
    }
    else if (list->TokenArray[*index]->type == t_float) // float - e.g. 0.5
    {
        (*index)++;
        if (callParams(list, index) == false)
        {
            return false;
        }
    }
    else if (list->TokenArray[*index]->type == t_string) // string - e.g. hello world!
    {
        (*index)++;
        if (callParams(list, index) == false)
        {
            return false;
        }
    }
    /* <var> */
    else if (list->TokenArray[*index]->type == t_varId) // variable - e.g. $id
    {
        (*index)++;
        if (callParams(list, index) == false)
        {
            return false;
        }
    }
    else
    {
        errorCode = SYNTAX_ERR;
        return false;
    }
    return true;
}

// <params> -> , <type> <var> <params> || eps
bool params(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        debug_log("FNC-DECL FUNCTION PARAMS rPAR %i \n", *index);
        return true;
    }

    if (list->TokenArray[*index]->type == t_comma) // -> , [comma]
    {
        (*index)++;
        if (typeCheck(list, index) == false) // call type check
        {
            return false;
        }
        debug_log("FNC-DECL FUNCTION PARAMS TYPE CHECK CALL %i \n", *index);
        (*index)++;
        if (list->TokenArray[*index]->type == t_varId)
        {
            (*index)++;
            debug_log("FNC-DECL FUNCTION PARAMS TYPE CHECK CALL NEXT %i \n", *index);
            if (params(list, index) == false) // -> <params>
            {
                return false;
            }
        }
        else
        {
            errorCode = SYNTAX_ERR;
            return false;
        }
    }
    else
    {
        errorCode = SYNTAX_ERR;
        return false;
    }
    return true;
}

// <param> -> <type> <var> <params> || eps
bool param(TokenList *list, int *index)
{
    if (list->TokenArray[*index]->type == t_rPar) // -> eps
    {
        debug_log("FNC-DECL FUNCTION PARAM rPAR %i \n", *index);
        return true;
    }
    if (typeCheck(list, index) == false) // call type check
    {
        return false;
    }
    debug_log("FNC-DECL FUNCTION PARAM TYPE CHECK CALL %i \n", *index);
    (*index)++;
    if (list->TokenArray[*index]->type == t_varId)
    {
        (*index)++;
        debug_log("FNC-DECL FUNCTION PARAM TYPE CHECK CALL NEXT %i ", *index);
        if (params(list, index) == false) // -> <params>
        {
            return false;
        }
    }
    else
    {
        errorCode = SYNTAX_ERR;
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
            errorCode = SYNTAX_ERR;
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
            errorCode = SYNTAX_ERR;
            return false;
        }
    default:
        errorCode = SYNTAX_ERR;
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
        errorCode = SYNTAX_ERR;
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
    errorCode = SYNTAX_ERR;
    return false;
}

// <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
bool functionDeclare(TokenList *list, int *index)
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
                debug_log("FNC-DECL FUNCTION PARAM %i \n", *index);
                if (param(list, index) == false)
                {
                    return false;
                }
                debug_log("FNC-DECL FUNCTION POST PARAM %i ", *index);
                if (list->TokenArray[*index]->type == t_rPar)
                {
                    (*index)++;
                    if (list->TokenArray[*index]->type == t_colon)
                    {
                        (*index)++;
                        debug_log("FNC-DECL FUNCTION TYPE %i ", *index);
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
                                debug_log("FNC-DECL FUNCTION STAT LIST %i ", *index);
                                if (statList(list, index) == false)
                                {
                                    return false;
                                }
                            }
                            if (list->TokenArray[*index]->type == t_rCurl)
                            {
                                debug_log("FNC-DECL FUNCTION END %i ", *index);
                                return true;
                            }
                        }
                    }
                }
            }
        }
        errorCode = SYNTAX_ERR;
        return false;
    }
    return true; // -> eps
}

// <st-list> -> <stat> <st-list> || eps
bool statList(TokenList *list, int *index)
{
    if (statement(list, index) == false)
    {
        return false;
    }
    (*index)++;
    if (list->TokenArray[*index]->type == t_rCurl) // end of statement with st-list or function declare - DON'T REMOVE!
    {
        return true;
    }
    if (statList(list, index) == false)
    {
        return false;
    }
    debug_log("ST-LIST %i ", *index);
    debug_log("%s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
    return true;
}

// <stat> -> if || while || assign || return || eps
bool statement(TokenList *list, int *index)
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
                        if (statList(list, index) == false)
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
                                    if (statList(list, index) == false)
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
            errorCode = SYNTAX_ERR;
            return false;
        }
        break;
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
                        if (statList(list, index) == false)
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
            errorCode = SYNTAX_ERR;
            return false;
        }
        break;
    case t_return: // return <expr> ;
        (*index)++;
        // precendAnalyser();
        if (list->TokenArray[*index]->type == t_semicolon)
        {
            // errorCode = SUCCESS;
            return true;
        }
        else
        {
            errorCode = SYNTAX_ERR;
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
            debug_log("\nINDEX: %s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
            (*index)++;
            debug_log("\nINDEX: %s\n", TOKEN_TYPE_STRING[list->TokenArray[*index]->type]);
            if (list->TokenArray[*index]->type == t_semicolon) // <r-side> -> eps
            {
                return true;
            }
            else if (list->TokenArray[*index]->type == t_assign) // <r-side> -> = <expr>
            {
                (*index)++;
                if (list->TokenArray[*index]->type == t_semicolon) // <assign> -> <var> =; => ERROR
                {
                    errorCode = SYNTAX_ERR;
                    return false;
                }
                while (list->TokenArray[*index]->type != t_semicolon) // temporary solution, before precedent Analyser is done
                {
                    (*index)++;
                }
                // precendAnalyser();
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
                errorCode = SYNTAX_ERR;
                return false;
            }
        }
        else // <assign> -> <expr> || <stat> -> eps
        {
            /* TODO - EPS STAT
            Teoreticky se to dá vyřešit, že se budu ptát, jestli tady nejsou nějaký klíčový tokeny a podle toho postupovat,
            třeba t_function mi to shodí return, protože se bude jednat o function declare */

            // (*index)++;
            // precendAnalyser();
        }
        break;
    }
    return true;
}

// <seq-stats> -> <stat> <fnc-decl> <seq-stats> || eps
bool seqStats(TokenList *list, int *index)
{
    // debug_log("SEQ-STAT %i ", *index);
    bool end;
    end = statement(list, index);
    if ((*index) == list->length || end == false || list->TokenArray[*index]->type == t_EOF)
    {
        debug_log("End of program\n");
        return true;
    }
    end = functionDeclare(list, index);
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
    if (seqStats(list, index) == false)
    {
        return false;
    }
    return true;
}

// <prog> -> <prolog> <seq-stats> <epilog>
bool checkSyntax(TokenList *list, int *index)
{
    debug_log("PROGRAM %i\n", *index);
    if (seqStats(list, index) == false)
    {
        return false;
    }
    return true;
}

bool synAnalyser(TokenList *list)
{
    int index = 0;
    if (!list->TokenArray[0]) // what does this do?
    {
        errorCode = SYNTAX_ERR;
        return false;
    }
    /* START OF RECURSIVE DESCENT */
    if (checkSyntax(list, &index) == false)
    {
        return false;
    }
    return true;
}