#include "lex-analyzer.h"
#include "sem-analyzer.h"
#include "error-codes.h"
#include "symtable.h"
#include "stack.h"
#include "devel.h"

#include <stdbool.h>
#include <string.h>

ht_table_t *initFncSymtable()
{
    ht_table_t fncSymtable;
    ht_init(fncSymtable);
    /*add each integrated function*/
    // "reads"
    // "readi"
    // "readf"
    // "write"
    // "floatval"
    // "intval"
    // "strval"
    // "strlen"
    // "substring"
    // "ord"
    // "chr"
    return NULL;
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

    param_info_t *requiredParam = data->function->data.fnc_data.params;
    AST_fnc_param *tmpParam = data->params;
    while (tmpParam != NULL) {
        if (requiredParam != NULL) { // no more required params
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
