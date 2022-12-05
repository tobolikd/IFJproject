#include "lex_analyzer.h"
#include "sem-analyzer.h"
#include "error-codes.h"
#include "symtable.h"
//#include "stack.h"
#include "devel.h"
#include "syn-analyzer.h"

#include <stdbool.h>
#include <string.h>


/// AUXILIARY FUNCTIONS

ht_table_t *initFncSymtable()
{
    ht_table_t *fncSymtable = ht_init();
    /*all integrated fnctions declaration*/

    // "reads"
    ht_insert(fncSymtable, "reads", null_string_t, true);

    // "readi"
    ht_insert(fncSymtable, "readi", null_int_t, true);

    // "readf"
    ht_insert(fncSymtable, "readf", null_float_t, true);
    // "write"
    ht_insert(fncSymtable, "write", void_t, true);

    // "strlen"
    ht_item_t *strlenItem = ht_insert(fncSymtable, "strlen", int_t, true);
    ht_paramAppend(strlenItem, "s", string_t);

    // "substring"
    ht_item_t  *substringItem = ht_insert(fncSymtable, "substring", string_t, true);
    ht_paramAppend(substringItem, "s", string_t);
    ht_paramAppend(substringItem, "i", int_t);
    ht_paramAppend(substringItem, "j", int_t);

    // "ord"
    ht_item_t  *ordItem = ht_insert(fncSymtable, "ord", int_t, true);
    ht_paramAppend(ordItem, "c", string_t);

    // "chr"
    ht_item_t  *chrItem = ht_insert(fncSymtable, "chr", string_t, true);
    ht_paramAppend(chrItem, "i", int_t);
    return (fncSymtable);
}


var_type_t typeforFnDec(TokenList *list, int *index) {
    switch (list->TokenArray[*index]->type) {
        case t_nullType:
            // debug_log("in NULL TYPE\n");
            if (!strcmp(list->TokenArray[*index]->data, "int")) {
                return null_int_t;
            } else if (!strcmp(list->TokenArray[*index]->data, "float")) {
                return null_float_t;
            } else if (!strcmp(list->TokenArray[*index]->data, "string")) {
                return null_string_t;
            } else {
                errorCode = SYNTAX_ERR;
                return error;
            }
        case t_type:
            // debug_log("in TYPE\n");
            if (!strcmp(list->TokenArray[*index]->data, "int")) {
                return int_t;
            } else if (!strcmp(list->TokenArray[*index]->data, "float")) {
                return float_t;
            } else if (!strcmp(list->TokenArray[*index]->data, "string")) {
                return string_t;
            } else {
                errorCode = SYNTAX_ERR;
                return error;
            }
        default:
            errorCode = SYNTAX_ERR;
            return error;
    }
}

var_type_t functionTypeForFunDec(TokenList *list, int *index)
{
    if (!strcmp(list->TokenArray[*index]->data, "void"))
    {
        // debug_log("in void\n");
        if (list->TokenArray[*index]->type == t_type)
        {
            return void_t;
        }
        errorCode = SYNTAX_ERR;
        return error;
    }
    else if (list->TokenArray[*index]->type == t_type || list->TokenArray[*index]->type == t_nullType)
    {
        // debug_log("in FNC TYPE\n");
        return typeforFnDec(list, index);
    }
    errorCode = SYNTAX_ERR;
    return error;
}

bool checkReturn(TokenList *list, int *index, ht_item_t *currFncDeclare) {
    int nestedLevel = 1;

    if (list->TokenArray[*index]->type != t_lCurl) {
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        debug_log("block expression doesnt start with \"{\"\n");
        return false;
    }

    (*index)++;

    // go to end of function declare
    while (nestedLevel != 0) {
        switch (list->TokenArray[*index]->type) {
            case t_EOF:
                errorCode = SYNTAX_ERR;
                debug_log("not ended function declare\n");
                return false;
            case t_lCurl:
                nestedLevel++;
                break;
            case t_rCurl:
                nestedLevel--;
                break;
            case t_return:
                // check if void returns nothing
                if (currFncDeclare->fnc_data.returnType == void_t) {
                    if (list->TokenArray[(*index) + 1]->type != t_semicolon) {
                        THROW_ERROR(SEMANTIC_RETURN_ERR, list->TokenArray[*index]->lineNum);
                        debug_log("void function has return statement with expression\n");
                        return false;
                    }
                } else { // check that non void function has return value
                    if (list->TokenArray[(*index) + 1]->type == t_semicolon) {
                        THROW_ERROR(SEMANTIC_RETURN_ERR,list->TokenArray[*index]->lineNum);
                        debug_log("non void function has empty return\n");
                        return false;
                    }
                }
                break;
            case t_function:
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
                debug_log("nested function declaration\n");
                return false;
            default:
                break;
        }
        (*index)++;
    }

    // undo getting culry bracket
    (*index)--;
    // no incompatibility found
    return true;
}

/// ! AUXILIARY FUNCTIONS

ht_table_t *PutFncsDecToHT(TokenList *list, ht_table_t *fncSymtable) {
    int index = 0;
    ht_item_t *currFncDeclare = NULL;

    //going through all tokens
    while(list->TokenArray[index]->type != t_EOF){
        //finding token with type t_function (declaration of fction)
        if(list->TokenArray[index]->type == t_function){
            index++;

            if(list->TokenArray[index]->type != t_functionId){errorCode = SYNTAX_ERR; return NULL;}

            //creating item with functionID as param, type is just temporary set to void_t, will change it at the end of the while loop
            currFncDeclare = ht_insert(fncSymtable, list->TokenArray[index]->data, void_t, true);

            //redeclaration of fction happened
            if(currFncDeclare == NULL){
                errorCode = SEMANTIC_FUNCTION_DEFINITION_ERR;
                debug_log("redeclaration of fction %i", errorCode);
                return NULL;
            }
            //debug_log(" item identifier %s\n", currFncDeclare->identifier);

            (index)++;
            //checking if next token is lPar
            if(list->TokenArray[index]->type == t_lPar) {
                (index)++;
                //if there are no params, skip adding params and jump to assign the return_type
                if(list->TokenArray[index]->type != t_rPar){
                    //adding params
                    //it there is error typeCheck fction sets the errorcode
                    typeCheck(list, &index);

                    if(errorCode == SYNTAX_ERR){ return NULL;}

                    var_type_t tmpParamType = typeforFnDec(list,&index);

                    (index)++;
                    if (list->TokenArray[index]->type != t_varId) { errorCode = SYNTAX_ERR;return NULL;}

                    //appending first param to our function
                    ht_paramAppend(currFncDeclare, list->TokenArray[index]->data, tmpParamType);
                    //debug_log("param %s \n", tmp->fnc_data.params->varId);
                    (index)++;
                    //there are more params
                    if(list->TokenArray[index]->type != t_rPar) {
                        if (list->TokenArray[index]->type == t_comma) {
                            do {
                                (index)++;

                                if (!typeCheck(list, &index)) { return NULL; } //setting errcode in the function already

                                tmpParamType = typeforFnDec(list,&index);

                                (index)++;
                                if (list->TokenArray[index]->type != t_varId) {errorCode = SYNTAX_ERR;return NULL;}

                                ht_paramAppend(currFncDeclare, list->TokenArray[index]->data, tmpParamType);
                                //debug_log("another param %s\n", currFncDeclare->fnc_data.params->next->varId);
                                (index)++;
                                //now the token can only be comma or ')'
                                if (list->TokenArray[index]->type != t_rPar && list->TokenArray[index]->type != t_comma) {errorCode = SYNTAX_ERR; return NULL;}

                                //if the next token isnt rPar, we continue in the loop and attach new params.
                            } while (list->TokenArray[index]->type != t_rPar);
                        }
                        //next token after varID is not comma
                        else {
                            errorCode = SYNTAX_ERR;
                            return NULL;
                        }
                    }
                }
                (index)++;
                if(list->TokenArray[index]->type == t_colon){

                    (index)++;
                    functionTypeForFunDec(list, &index);

                    if(errorCode == SYNTAX_ERR){return NULL;}

                    //setting return type of our newly declared function
                    currFncDeclare->fnc_data.returnType = functionTypeForFunDec(list, &index);
                    debug_log("return type %i\n", currFncDeclare->fnc_data.returnType);
                    (index)++;
                }

                if(checkReturn(list, &index, currFncDeclare) == false) {
                    debug_log("checkReturn returned false\n");
                    return NULL;
                }
                debug_log("got through return check ended on line %d\n", list->TokenArray[index]->lineNum);
            }
        }
        //raising index and going back to loop, where we check if the token is last token
        index++;
    }
    //we checked the whole TokenList last token was EOF, return the created symtable
    return (fncSymtable);
}

ht_table_t *InitializedHTableFnctionDecs(TokenList *list){
    ht_table_t *table = initFncSymtable();
    if(PutFncsDecToHT(list, table)==NULL)
    {
        debug_log("Check return failed \n");
        return NULL;
    }
    if (errorCode != SUCCESS){ return NULL;}
    return table;
}

bool cmpParamTypes(var_type_t symtable, AST_param_type ast) {
    switch (ast) {
        case AST_P_INT:
            if (symtable == int_t || symtable == null_int_t)
                return true;
            return false;
        case AST_P_STRING:
            if (symtable == string_t || symtable == null_string_t)
                return true;
            return false;
        case AST_P_FLOAT:
            if (symtable == float_t || symtable == null_float_t)
                return true;
            return false;
        case AST_P_NULL:
            if (symtable == null_int_t || symtable == null_float_t || symtable == null_string_t)
                return true;
            return false;
        default: // AST_P_VAR and others
            return false;
    }
    return false;
}

bool checkFncCall(AST_function_call_data *data) {
    // not declared function
    if (data->function == NULL) { ERR_FNC_NOT_DECLARED(data->functionID); return false; }

    if (0 == strcmp(data->function->identifier, "write"))
        return true; // built in function with unlimited params
    // all ast types are allowed

    param_info_t *requiredParam = data->function->fnc_data.params;
    AST_fnc_param *tmpParam = data->params;
    while (tmpParam != NULL) {
        if (requiredParam == NULL) { // no more required params
            ERR_FNC_PARAM_COUNT(data->functionID);
#if DEBUG == 1
            printAstFnc(data);
#endif
            return false;
        }

        if (requiredParam->type == void_t) {
            ERR_INTERNAL(checkFncCall, "void parameter type in declared function\n\tfunction: %s\n\tparam: %s", data->functionID, requiredParam->varId);
#if DEBUG == 1
            printAstFnc(data);
#endif
            return false;
        }

        if (tmpParam->type != AST_P_VAR) { // check constant type
            if (!cmpParamTypes(requiredParam->type, tmpParam->type)) {
                ERR_FNC_PARAM_TYPE(data->functionID);
#if DEBUG == 1
                printAstFnc(data);
#endif
                return false;
            }
        }

        requiredParam = requiredParam->next;
        tmpParam = tmpParam->next;
    }
    if (requiredParam != NULL) { // more params needed
        ERR_FNC_PARAM_COUNT(data->functionID);
#if DEBUG == 1
        printAstFnc(data);
#endif
        return false;
    }
    return true;
}
