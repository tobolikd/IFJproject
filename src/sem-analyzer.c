#include "lex-analyzer.h"
#include "sem-analyzer.h"
#include "error-codes.h"
#include "symtable.h"
//#include "stack.h"
#include "devel.h"
#include "syn-analyzer.h"

#include <stdbool.h>
#include <string.h>


/// AUXILIARY FUNCTIONS

ht_table_t *integratedFncSymtable()
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
    ht_param_append(strlenItem, "s", string_t);

    // "substring"
    ht_item_t  *substringItem = ht_insert(fncSymtable, "substring", string_t, true);
    ht_param_append(substringItem, "s", string_t);
    ht_param_append(substringItem, "i", int_t);
    ht_param_append(substringItem, "j", int_t);

    // "ord"
    ht_item_t  *ordItem = ht_insert(fncSymtable, "ord", int_t, true);
    ht_param_append(ordItem, "c", string_t);

    // "chr"
    ht_item_t  *chrItem = ht_insert(fncSymtable, "chr", string_t, true);
    ht_param_append(chrItem, "i", int_t);

    return (fncSymtable);
}


var_type_t typeforFnDec(TokenList *list, int *index) {
    switch (list->TokenArray[*index]->type) {
        case t_nullType:
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
    if (!strcmp(list->TokenArray[*index]->data, "void")) {
        if (list->TokenArray[*index]->type == t_type) {
            return void_t;
        }
        THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
        return error;
    }
    else if (list->TokenArray[*index]->type == t_type || list->TokenArray[*index]->type == t_nullType) {
        return typeforFnDec(list, index);
    }
    THROW_ERROR(SYNTAX_ERR, list->TokenArray[*index]->lineNum);
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

ht_table_t *FncDeclarations(TokenList *list, ht_table_t *fncSymtable) {

    int index = 0;
    ht_item_t *currFncDeclare = NULL;

    //going through all tokens
    while(CURR_TOKEN_TYPE != t_EOF){
        //finding token with type t_function (declaration of function)
        if(CURR_TOKEN_TYPE == t_function){

            NEXT_TOKEN;

            if(CURR_TOKEN_TYPE != t_functionId) {
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[index]->lineNum)
                return NULL;
            }
            //creating item with functionID as its identifier, type is just temporary set to void_t, will be changed at the end of the while loop
            currFncDeclare = ht_insert(fncSymtable, list->TokenArray[index]->data, void_t, true);

            if(currFncDeclare == NULL){ //redeclaration of function
                THROW_ERROR(SEMANTIC_FUNCTION_DEFINITION_ERR,list->TokenArray[index]->lineNum)
                debug_print("redeclaration of fction %i", errorCode);
                return NULL;
            }

            NEXT_TOKEN;

            //next token must be '('
            if(CURR_TOKEN_TYPE != t_lPar) {
                THROW_ERROR(SYNTAX_ERR, list->TokenArray[index]->lineNum)
                return NULL;
            }

            NEXT_TOKEN;

            //ADDING PARAMS
            //if there are no params, skip adding params and jump to assigning the return_type
            if(CURR_TOKEN_TYPE != t_rPar){

                if(!typeCheck(list, &index)) {
                return NULL; //typeCheck function sets the errorcode
                }
                var_type_t paramType = typeforFnDec(list,&index);

                NEXT_TOKEN;

                if (CURR_TOKEN_TYPE != t_varId) {
                    THROW_ERROR(SYNTAX_ERR,list->TokenArray[index]->lineNum);
                    return NULL;
                }
                //appending first param to our function
                ht_param_append(currFncDeclare, list->TokenArray[index]->data, paramType);

                NEXT_TOKEN;

                if(CURR_TOKEN_TYPE != t_rPar) { //there are more params
                    if (CURR_TOKEN_TYPE != t_comma) {
                        THROW_ERROR(SYNTAX_ERR,list->TokenArray[index]->lineNum)
                        return NULL;
                    }
                    //appending all other params with do-while cycle
                    do {
                            NEXT_TOKEN;

                            if (!typeCheck(list, &index)) {
                                return NULL; //setting errcode in the function already
                            }
                            paramType = typeforFnDec(list,&index);

                            NEXT_TOKEN;

                            if (CURR_TOKEN_TYPE != t_varId) {
                                THROW_ERROR(SYNTAX_ERR,list->TokenArray[index]->lineNum);
                                return NULL;
                            }
                            ht_param_append(currFncDeclare, list->TokenArray[index]->data, paramType);

                            NEXT_TOKEN;

                            //now the token can only be comma or ')'
                            if (CURR_TOKEN_TYPE != t_rPar && CURR_TOKEN_TYPE != t_comma) {
                                THROW_ERROR(SYNTAX_ERR,list->TokenArray[index]->lineNum)
                                return NULL;
                            }
                    } while (CURR_TOKEN_TYPE != t_rPar);   //if the next token isn't ')', we continue in the loop, check and attach new params.
                }
            }
            //params were processed /none/ /1 param/ /more params/

            NEXT_TOKEN;

            if(CURR_TOKEN_TYPE != t_colon) {
                THROW_ERROR(SYNTAX_ERR,list->TokenArray[index]->lineNum)
                return NULL;
            }

            NEXT_TOKEN;

            functionTypeForFunDec(list, &index);//sets the errorcode
            if(errorCode != SUCCESS) {
                return NULL;
            }
            //setting return type of our newly declared function
            currFncDeclare->fnc_data.returnType = functionTypeForFunDec(list, &index);
            debug_log("return type %i\n", currFncDeclare->fnc_data.returnType);

            NEXT_TOKEN;

            if(checkReturn(list, &index, currFncDeclare) == false) {
                debug_log("checkReturn returned false\n");
                return NULL;
            }
            debug_log("got through return check ended on line %d\n", list->TokenArray[index]->lineNum);
        }
        NEXT_TOKEN;  //raising index and going back to loop, where we check if the token is last token
    }
    //we checked the whole TokenList, return the created symtable
    return (fncSymtable);
}

ht_table_t *FncDeclarationTable(TokenList *list) {
    ht_table_t *table = integratedFncSymtable();
    if(FncDeclarations(list, table) == NULL) {
        debug_log("Check return failed \n");
        return NULL;
    }
    if (errorCode != SUCCESS) {
        return NULL;
    }
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
