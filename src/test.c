#include "stack.h"
#include "ast.h"
#include "code-gen.h"
#include "error-codes.h"

#include <stdlib.h>


int main() {
    errorCode = SUCCESS;
    stack_ast_t *ast = (stack_ast_t *) malloc(sizeof(stack_ast_t));
    stack_ast_init(ast);

    int tmpInt = 5;
    char *tmpString = "ahoj#some string\tpad";

    ht_item_t var;
    var.identifier = "a";

    stack_ast_push_b(ast, ast_item_const(AST_END_BLOCK, NULL));
    stack_ast_push_b(ast, ast_item_const(AST_ELSE, NULL));
    stack_ast_push_b(ast, ast_item_const(AST_END_BLOCK, NULL));

    // nested
    stack_ast_push_b(ast, ast_item_const(AST_END_BLOCK, NULL));
    stack_ast_push_b(ast, ast_item_const(AST_ELSE, NULL));
    stack_ast_push_b(ast, ast_item_const(AST_END_BLOCK, NULL));
    stack_ast_push_b(ast, ast_item_const(AST_STRING, tmpString));
    stack_ast_push_b(ast, ast_item_const(AST_IF, NULL));

    stack_ast_push_b(ast, ast_item_const(AST_VAR, &var));
    stack_ast_push_b(ast, ast_item_const(AST_IF, NULL));


    /* function declare and return
    ht_item_t fnc_declare;
    fnc_declare.identifier = "write";

    stack_ast_push_b(ast, ast_item_const(AST_END_BLOCK, NULL));
    stack_ast_push_b(ast, ast_item_const(AST_RETURN_VOID, NULL));
    stack_ast_push_b(ast, ast_item_const(AST_FUNCTION_DECLARE, &fnc_declare));
    */
    ht_table_t *fncSymtable = ht_init();
    codeGenerator(ast, fncSymtable);

    free(ast);
    ht_delete_all(fncSymtable);
    return errorCode;
}
