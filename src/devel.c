#include "devel.h"
#include "error-codes.h"
#include "ast.h"

#define AST_TYPE(TYPE) debug_log("\t|- type: |%s|\n", TYPE)
#define AST_DATA(DATA) debug_log("\t|- data: %s\n", DATA)

/// AUXILIARY FUNCTIONS

void printAstParam(AST_fnc_param *param) {
    switch (param->type) {
        case AST_P_INT:
            debug_log("\t\t\t|- Type: |int|\n");
            debug_log("\t\t\t|- Value: %d\n", param->data->intValue);
        	break;
        case AST_P_VAR:
            debug_log("\t\t\t|- Type: |string|\n");
            debug_log("\t\t\t|- Value: symtable variable");
            debug_log("\t\t\t\t|- Id: %s", param->data->variable->identifier);
        	break;
        case AST_P_NULL:
            debug_log("\t\t\t|- Type: |null|\n");
        	break;
        case AST_P_FLOAT:
            debug_log("\t\t\t|- Type: |float|\n");
            debug_log("\t\t\t|- Value: %f\n", param->data->floatValue);
        	break;
        case AST_P_STRING:
            debug_log("\t\t\t|- Type: |string|\n");
            debug_log("\t\t\t|- Value: \"%s\"\n", param->data->stringValue);
        	break;
        default:
            debug_log("\t\t\t|-Type: UNKNOWN PARAM TYPE!\n");
            break;
    }
}

void printAstFnc(AST_function_call_data *data) {
    debug_log("\t\t|- Id: \"%s\"\n", data->function->identifier);
    AST_fnc_param *tmp = data->params;
    for (int i = 0; tmp != NULL; i++) {
        printAstParam(tmp);
        tmp = tmp->next;
    }
}

/// !AUXILIARY FUNCTIONS

void printAstItem(AST_item *item) {
#if DEBUG == 0
    debug_print("\nWARNING(internal): called printAstItem with DEBUG 0, run with DEBUG 1 for more info\n");
#endif

    if (item == NULL) { debug_log("Item is NULL"); return; }

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
            debug_log("\t|- data: %d\n", item->data->intValue);
            break;    // data - int intValue
        case AST_STRING:
            AST_TYPE("const string");
            AST_DATA(item->data->stringValue);
            break; // data - char *stringValue
        case AST_FLOAT:
            AST_TYPE("const float");
            debug_log("\t|- data: %f\n", item->data->floatValue);
            break;  // data - float floatValue
        case AST_NULL:
            AST_TYPE("null");
            break;

        // fnc declaration
        case AST_FUNCTION_DECLARE:
            AST_TYPE("function declaration");
            AST_DATA(item->data->function->identifier);
            break;   // data - ht_item_t *function

        // program control (jumps)
        case AST_FUNCTION_CALL:
            AST_TYPE("function call");
            printAstFnc(item->data->functionCallData);
            break;  // data - AST_function_call_data *functionCallData
        case AST_RETURN:
            AST_TYPE("return");
            if (item->data->blank == true)
                AST_DATA("empty return statement");
            else
                AST_DATA("return statement with expression");
            break; // data - bool blank
        case AST_IF:
            AST_TYPE("if");
            break;
        case AST_ELSE:
            AST_TYPE("else");
            break;
        case AST_WHILE:
            AST_TYPE("while");
            break;

        case AST_END_BLOCK:
            AST_TYPE("block end");
            break;

        default:
            AST_TYPE("Unknown type!");
            break;
    }
}