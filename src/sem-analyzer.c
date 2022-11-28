#include "lex-analyzer.h"
#include "sem-analyzer.h"
#include "error-codes.h"
#include "symtable.h"
//#include "stack.h"
#include "devel.h"
#include "syn-analyzer.h"

#include <stdbool.h>
#include <string.h>


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
    debug_log("%s \n", ht_search(fncSymtable, "write")->identifier);

    // "strlen"
    ht_item_t *strlenItem = ht_insert(fncSymtable, "strlen", int_t, true);
    ht_param_append(strlenItem, "strlenParam", int_t);

    // "substring"
    ht_item_t  *substringItem = ht_insert(fncSymtable, "substring", string_t, true);
    ht_param_append(substringItem, "subStringParam", string_t);
    ht_param_append(substringItem, "endingIndex", int_t);
    ht_param_append(substringItem, "startingIndex", int_t);

    debug_log("%s \n", substringItem->identifier);
    debug_log("%s \n", substringItem->fnc_data.params->varId);
    // "ord"
    ht_item_t  *ordItem = ht_insert(fncSymtable, "ord", int_t, true);
    ht_param_append(ordItem, "ordinaryValueString", string_t);

    // "chr"
    ht_item_t  *chrItem = ht_insert(fncSymtable, "chr", string_t, true);
    ht_param_append(chrItem, "chrString", string_t);
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

ht_table_t *PutFncsDecToHT(TokenList *list, ht_table_t *fncSymtable){
    int index = 0;
    while(list->TokenArray[index]->type != t_EOF){
        if(list->TokenArray[index]->type == t_function){
            index++;
            if(list->TokenArray[index]->type != t_functionId){errorCode = SYNTAX_ERR; return NULL;}
            ht_item_t *tmp = ht_insert(fncSymtable, list->TokenArray[index]->data, void_t, true); //just temporary type void_t, will change it at the end
            if(tmp == NULL){//redeclaration of fction.
                errorCode = SEMANTIC_FUNCTION_DEFINITION_ERR;
                debug_log("redeclaration of fction %i", errorCode);
                return NULL;}
            //debug_log(" item identifier %s\n", tmp->identifier);

            (index)++;
            if(list->TokenArray[index]->type == t_lPar) {
                (index)++;
                if(list->TokenArray[index]->type != t_rPar){ //if there are no params, skip adding params.
                    //adding params
                    typeCheck(list, &index);

                    if(errorCode == SYNTAX_ERR){ return NULL;}

                    var_type_t tmpParamType = typeforFnDec(list,&index);

                    (index)++;

                    if (list->TokenArray[index]->type != t_varId) { errorCode = SYNTAX_ERR;return NULL;}

                    ht_param_append(tmp, list->TokenArray[index]->data, tmpParamType);
                    debug_log("param %s \n", tmp->fnc_data.params->varId);
                    (index)++;

                    if(list->TokenArray[index]->type != t_rPar) {
                        if (list->TokenArray[index]->type == t_comma) {
                            do {
                                (index)++;

                                if (!typeCheck(list, &index)) { return NULL; } //setting errcode in the function already

                                tmpParamType = typeforFnDec(list,&index);
                                (index)++;

                                if (list->TokenArray[index]->type != t_varId) {errorCode = SYNTAX_ERR;return NULL;}

                                ht_param_append(tmp, list->TokenArray[index]->data, tmpParamType);
                                //debug_log("another param %s\n", tmp->fnc_data.params->next->varId);
                                (index)++;

                                if (list->TokenArray[index]->type != t_rPar && list->TokenArray[index]->type != t_comma) {errorCode = SYNTAX_ERR; return NULL;}

                                //nerovna se prave zav, muzeme dal prirazovat, projeli jsme dowhilem, protoze tak by mohl byt dalsi znak ), tak to projde a zjisti chybu pri prvnim pruchodu
                            } while (list->TokenArray[index]->type != t_rPar);
                        }
                        else {//next token after varID is not comma
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

                    tmp->fnc_data.returnType = functionTypeForFunDec(list, &index);
                    debug_log("return type %i\n", tmp->fnc_data.returnType);
                    (index)++;
                }
            }
        }
        index++;
    }
    return (fncSymtable);
}

ht_table_t *InitializedHTableFnctionDecs(TokenList *list){
    ht_table_t *table = initFncSymtable();
    PutFncsDecToHT(list, table);
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
