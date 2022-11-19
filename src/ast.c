/* @file ast.h
 *
 * @brief contains data structures and functions for AST
 *
 * @author David Tobolik (xtobol06)
 */

#include "ast.h"
#include "error-codes.h"
#include <stdlib.h>

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
        if (item->data->functionCallData != NULL)
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

    free(data->functionId);

    AST_fnc_param *deleted = data->params;
    AST_fnc_param *next;

    while (deleted != NULL) {
        next = deleted->next;
        if (deleted->type == AST_STRING)
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
