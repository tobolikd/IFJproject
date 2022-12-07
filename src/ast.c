/* @file ast.c
 *
 * @brief implementation of ast interface
 *
 * @author David Tobolik (xtobol06)
 */

#include "ast.h"
#include "error_codes.h"
#include <stdlib.h>
#include <string.h>

void astItemDestr(AST_item *item) {
    if (item == NULL)
        return;

    if (item->data == NULL) {
        free(item);
        return;
    }

    // free function call data
    if (item->type == AST_FUNCTION_CALL) {
        fncCallDataDestr(item->data->functionCallData);
    }

    // free function declare data
    if (item->type == AST_FUNCTION_DECLARE) {
        free(item->data->functionDeclareData);
    }

    // free string constant
    if (item->type == AST_STRING) {
        free(item->data->stringValue);
    }

    // free value and item
    free(item->data);
    free(item);
}

// free all parametres and function id
void fncCallDataDestr(AST_function_call_data *data) {
    if (data == NULL) {
        debug_print("WARNING(internal): AST item of type AST_FUNCTION_CALL has no funtion call data\n");
        return;
    }

    AST_fnc_param *deleted = data->params;
    AST_fnc_param *next;

    // delete parametres of function
    while (deleted != NULL) {
        next = deleted->next;
        // if string, free string
        if (deleted->type == AST_P_STRING)
            free(deleted->data->stringValue);
        free(deleted->data);
        free(deleted);
        deleted = next;
    }
    free(data);
}

#define ALLOCATE_AST_DATA                   \
    do {                                    \
    new->data = malloc(sizeof(AST_data));   \
    CHECK_MALLOC_PTR(new->data);            \
    } while (0)

AST_item *astItemConst(AST_type type, void *data) {
    AST_item *new = (AST_item *) malloc(sizeof(AST_item));
    CHECK_MALLOC_PTR(new);

    new->type = type;

    // according to type assign data
    switch (type) {

        // arithmetic operations
        case AST_ADD:
        case AST_SUBTRACT:
        case AST_DIVIDE:
        case AST_MULTIPLY:
        // string operation
        case AST_CONCAT:
        // relation operations
        case AST_EQUAL:
        case AST_NOT_EQUAL:
        case AST_GREATER:
        case AST_GREATER_EQUAL:
        case AST_LESS:
        case AST_LESS_EQUAL:
        // data control
        case AST_ASSIGN:
            new->data = NULL;
            break;
        case AST_VAR:
            ALLOCATE_AST_DATA;
            new->data->variable = (ht_item_t *) data;
            break;    // data - ht_item_t *variable

        // constants
        case AST_INT:
            ALLOCATE_AST_DATA;
            new->data->intValue = *((int *) data);
            break;    // data - int intValue
        case AST_STRING:
            ALLOCATE_AST_DATA;
            new->data->stringValue = (char *) malloc((strlen((char*) data) + 1) * sizeof(char));
            if (new->data->stringValue == NULL) {
                free(new);
                MALLOC_ERR();
                return NULL;
            }
            strcpy(new->data->stringValue, (char *) data);
            break; // data - char *stringValue
        case AST_FLOAT:
            ALLOCATE_AST_DATA;
            new->data->floatValue = *((double *) data);
            break;  // data - double floatValue
        case AST_NULL:
            new->data = NULL;
            break;

        // fnc declaration
        case AST_FUNCTION_DECLARE:
            ALLOCATE_AST_DATA;
            new->data->functionDeclareData = (AST_function_declare_data *) data;
            break;   // data - ht_item_t *function

        // program control (jumps)
        case AST_FUNCTION_CALL:
            ALLOCATE_AST_DATA;
            new->data->functionCallData = (AST_function_call_data *) data;
            break;  // data - AST_function_call_data *functionCallData
        case AST_RETURN_EXPR:
        case AST_RETURN_VOID:
            new->data = NULL;
            break;
        case AST_IF:
        case AST_ELSE:
        case AST_WHILE:
        case AST_END_EXPRESSION:
        case AST_END_BLOCK:
            new->data = NULL;
            break;
        default:
            debug_log("ERROR(internal): in function astItemConst\n\tUnknown type!");
            new->data = NULL;
            break;
    }
    return new;
}

AST_function_declare_data *fncDeclareDataConst(ht_item_t *function, ht_table_t *varSymtable) {
    AST_function_declare_data *new = (AST_function_declare_data *) malloc(sizeof(AST_function_declare_data));
    CHECK_MALLOC_PTR(new);

    new->function = function;
    new->varSymtable = varSymtable;

    return new;
}

AST_function_call_data *fncCallDataConst(ht_table_t *fncSymtable, char *functionId) {
    AST_function_call_data *new = (AST_function_call_data *) malloc(sizeof(AST_function_call_data));
    CHECK_MALLOC_PTR(new);

    new->functionId = functionId;
    new->function = ht_search(fncSymtable, functionId);
    new->params = NULL;

    return new;
}

void fncCallDataAddParam(AST_function_call_data *data, AST_param_type type, void *param) {
    AST_fnc_param *new = malloc(sizeof(AST_fnc_param));
    CHECK_MALLOC(new);

    // null argument has no data
    if (type != AST_P_NULL) {
        new->data = malloc(sizeof(AST_param_data));

        if (new->data == NULL) {
            free(new);
            MALLOC_ERR();
            return;
        }
    }

    new->next = NULL;
    new->type = type;

    // copy param to parameter data
    switch (type) {
        case AST_P_INT:
            new->data->intValue = *((int*)param);
            break;

        case AST_P_VAR:
            new->data->variable = (ht_item_t *) param;
            break;

        case AST_P_NULL:
            new->data = NULL;
            break;

        case AST_P_FLOAT:
            new->data->floatValue = *((double*)param);
            break;

        case AST_P_STRING:
            new->data->stringValue = (char *) malloc((strlen((char*) param) + 1) * sizeof(char));
            CHECK_MALLOC(new->data->stringValue);
            strcpy(new->data->stringValue, (char *) param);
            break;

        default:
            errorCode = INTERNAL_ERR;
            debug_print("INTERNAL ERR - in fncCallDataAddParam():\n\tunknown type of fnc param (%d)\n", type);
            break;
    }

    // add param to end of parameter list
    AST_fnc_param **tmp = &data->params;
    while (*tmp != NULL) {
        tmp = &(*tmp)->next;
    }

    *tmp = new;
}
