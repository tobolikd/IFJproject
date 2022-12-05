/* @file devel.c
 *
 * @brief implementation of formatted prints
 *
 * @author David Tobolik (xtobol06)
 */

#include "devel.h"
#include "error_codes.h"
#include "ast.h"

#define AST_TYPE(TYPE) debug_log("\t|- type: |%s|\n", TYPE)
#define AST_DATA(DATA) debug_log("\t|- data: %s\n", DATA)

/// AUXILIARY FUNCTIONS

void print_ast_param(AST_fnc_param *param) {
    switch (param->type) {
        case AST_P_INT:
            debug_log("\t\t\t|- Type: |int|\n");
            debug_log("\t\t\t|- Value: %d\n", param->data->int_value);
        	break;
        case AST_P_VAR:
            debug_log("\t\t\t|- Type: |string|\n");
            debug_log("\t\t\t|- Value: symtable variable\n");
            debug_log("\t\t\t\t|- Id: %s\n", param->data->variable->identifier);
        	break;
        case AST_P_NULL:
            debug_log("\t\t\t|- Type: |null|\n");
        	break;
        case AST_P_FLOAT:
            debug_log("\t\t\t|- Type: |float|\n");
            debug_log("\t\t\t|- Value: %f\n", param->data->float_value);
        	break;
        case AST_P_STRING:
            debug_log("\t\t\t|- Type: |string|\n");
            debug_log("\t\t\t|- Value: \"%s\"\n", param->data->string_value);
        	break;
        default:
            debug_log("\t\t\t|-Type: UNKNOWN PARAM TYPE!\n");
            break;
    }
}

/// !AUXILIARY FUNCTIONS

void print_ast_fnc(AST_function_call_data *data) {
    debug_log("\t\t|- Id: \"%s\"\n", data->function->identifier);
    AST_fnc_param *tmp = data->params;
    for (int i = 0; tmp != NULL; i++) {
        print_ast_param(tmp);
        tmp = tmp->next;
    }
}

void print_ast_item(AST_item *item) {
#if DEBUG == 0
    debug_print("\nWARNING(internal): called print_ast_item with DEBUG 0, run with DEBUG 1 for more info\n");
#endif

    if (item == NULL) { debug_log("Item is NULL\n"); return; }

    debug_log("\nAST_item\n");

    switch (item->type) {

        // arithmetic operations
        case AST_ADD:
            AST_TYPE("+");
            break;
        case AST_SUBTRACT:
            AST_TYPE("-");
            break;
        case AST_DIVIDE:
            AST_TYPE("/");
            break;
        case AST_MULTIPLY:
            AST_TYPE("*");
            break;

        // string operation
        case AST_CONCAT:
            AST_TYPE(".");
            break;

        // relation operations
        case AST_EQUAL:
            AST_TYPE("===");
            break;
        case AST_NOT_EQUAL:
            AST_TYPE("!==");
            break;
        case AST_GREATER:
            AST_TYPE(">");
            break;
        case AST_GREATER_EQUAL:
            AST_TYPE(">=");
            break;
        case AST_LESS:
            AST_TYPE("<");
            break;
        case AST_LESS_EQUAL:
            AST_TYPE("<=");
            break;

        // data control
        case AST_ASSIGN:
            AST_TYPE("=");
            break;
        case AST_VAR:
            AST_TYPE("variable");
            AST_DATA(item->data->variable->identifier);
            break;    // data - ht_item_t *variable

        // constants
        case AST_INT:
            AST_TYPE("const int");
            debug_log("\t|- data: %d\n", item->data->int_value);
            break;    // data - int int_value
        case AST_STRING:
            AST_TYPE("const string");
            AST_DATA(item->data->string_value);
            break; // data - char *string_value
        case AST_FLOAT:
            AST_TYPE("const float");
            debug_log("\t|- data: %f\n", item->data->float_value);
            break;  // data - float float_value
        case AST_NULL:
            AST_TYPE("null");
            break;

        // fnc declaration
        case AST_FUNCTION_DECLARE:
            AST_TYPE("function declaration");
            AST_DATA(item->data->function_call_data->function->identifier);
            break;   // data - ht_item_t *function

        // program control (jumps)
        case AST_FUNCTION_CALL:
            AST_TYPE("function call");
            print_ast_fnc(item->data->function_call_data);
            break;  // data - AST_function_call_data *function_call_data
        case AST_RETURN_VOID:
            AST_TYPE("return (void)");
            break;
        case AST_RETURN_EXPR:
            AST_TYPE("return (expression)");
            break;
        case AST_IF:
            AST_TYPE("if");
            break;
        case AST_ELSE:
            AST_TYPE("else");
            break;
        case AST_WHILE:
            AST_TYPE("while");
            break;

        case AST_END_EXPRESSION:
            AST_TYPE("expression end");
            break;

        case AST_END_BLOCK:
            AST_TYPE("block end");
            break;

        default:
            AST_TYPE("Unknown type!");
            break;
    }
}

void print_ast_stack(stack_ast_t *stack) {
    stack_ast_item_t *tmp = stack->top;
    while (tmp != NULL) {
        print_ast_item(tmp->data);
        tmp = tmp->next;
    }
}

void print_code_block_stack(stack_code_block_t *stack) {
    debug_log("\n\nCODE BLOCK STACK:\n");
    stack_code_block_item_t *tmp = stack->top;
    while (tmp != NULL) {
        debug_log(" - type: %d\n", tmp->data->type);
        debug_log("  \\label_num: %d\n", tmp->data->label_num);
        tmp = tmp->next;
    }
}
