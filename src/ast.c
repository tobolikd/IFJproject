/* @file ast.h
 *
 * @brief contains data structures and functions for AST
 *
 * @author David Tobolik (xtobol06)
 */

#include "ast.h"
#include "error-codes.h"
#include <stdlib.h>
#include <string.h>

void ast_item_destr(AST_item *item) {
    if (item == NULL)
        return;

    if (item->data == NULL) {
        free(item);
        return;
    }

    // free function call data
    if (item->type == AST_FUNCTION_CALL) {
        function_call_data_destr(item->data->functionCallData);
        free(item->data->functionCallData);
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
void function_call_data_destr(AST_function_call_data *data) {
    if (data == NULL) {
        debug_print("WARNING(internal): AST item of type AST_FUNCTION_CALL has no funtion call data\n");
        return;
    }

    AST_fnc_param *deleted = data->params;
    AST_fnc_param *next;

    while (deleted != NULL) {
        next = deleted->next;
        if (deleted->type == AST_P_STRING)
            free(deleted->data->stringValue);
        free(deleted->data);
        free(deleted);
        deleted = next;
    }
}

AST_item *ast_item_const(AST_type type, AST_data *data) {
    AST_item *new = (AST_item *) malloc(sizeof(AST_item));
    CHECK_MALLOC_PTR(new);

    new->type = type;
    new->data = data;
    return new;
}

void fnc_call_data_init(AST_function_call_data *data, ht_item_t *function) {
    data->function = function;
    data->params = NULL;
}

void fnc_call_data_add_param(AST_function_call_data *data, AST_param_type type, void *param) {
    AST_fnc_param *new = malloc(sizeof(AST_fnc_param));
    CHECK_MALLOC(new);

    // null argument has no data
    if (type != AST_P_NULL) {
        new->data = malloc(sizeof(AST_param_data));

        if (new->data == NULL) {
            free(new);
            MALLOC_ERR;
            return;
        }
    }

    new->next = NULL;
    new->type = type;

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
            new->data->floatValue = *((float*)param);
            break;

        case AST_P_STRING:
            new->data->stringValue = (char *) malloc((strlen((char*) param) + 1) * sizeof(char));
            CHECK_MALLOC(new->data->stringValue);
            strcpy(new->data->stringValue, (char *) param);
            break;

        default:
            errorCode = INTERNAL_ERR;
            debug_print("INTERNAL ERR - in fnc_call_data_add_param():\n\tunknown type of fnc param (%d)\n", type);
            break;
    }

    AST_fnc_param **tmp = &data->params;
    while (*tmp != NULL) {
        tmp = &(*tmp)->next;
    }

    *tmp = new;
}
