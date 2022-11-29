#include "stack.h"
#include "ast.h"
#include "code-gen.h"
#include "preced-analyzer.h"
#include "lex-analyzer.h"
#include "error-codes.h"

#include <stdlib.h>

ht_table_t *fncTable;

int main() {
    errorCode = SUCCESS;

    FILE *tmpFile = fopen("../myTestFiles/test.txt","r");
    TokenList *testList;
    int index;
    ht_table_t *varTable;
    stack_ast_t ast;
    fncTable = ht_init();

    stack_ast_init(&ast);//init stack

    varTable = ht_init();//init symtables
    fncTable = ht_init();

    char a[] = "a";
    char b[] = "b";
    char foo[] = "foo";
    char doe[] = "doe";
    char poo[] = "poo";
    char param[] = "param";

    // ht_insert(varTable,a,int_t,false);//insert to symtables
    // ht_insert(varTable,b,int_t,false);

    // ht_param_append(ht_insert(fncTable,foo,int_t,true),param,float_t);
    
    // ht_insert(fncTable,poo,int_t,true);
    // ht_param_append(ht_search(fncTable,poo),param,float_t);
    // ht_param_append(ht_search(fncTable,poo),param,float_t);

    // ht_insert(fncTable,doe,int_t,true);

    index = 0;

    testList = lexAnalyser(tmpFile);

    parseExpression(testList,&index,varTable,&ast);

    // stack_ast_push(&ast, ast_item_const(AST_END_BLOCK, NULL));
//CODEGEN
    codeGenerator(&ast, fncTable);

//TEARDOWN
    while (!stack_ast_empty(&ast)) //pop all
        stack_ast_pop(&ast);

    ht_delete_all(fncTable);
    ht_delete_all(varTable);
    if (tmpFile != NULL)
        fclose(tmpFile);
    listDtor(testList);
    // int tmpInt = 5;
    // char *tmpString = "ahoj#some string\tpad";

    // ht_item_t var;
    // var.identifier = "a";

    // stack_ast_push(ast, ast_item_const(AST_END_BLOCK, NULL));
    // stack_ast_push(ast, ast_item_const(AST_ELSE, NULL));
    // stack_ast_push(ast, ast_item_const(AST_END_BLOCK, NULL));

    // // nested
    // stack_ast_push(ast, ast_item_const(AST_END_BLOCK, NULL));
    // stack_ast_push(ast, ast_item_const(AST_ELSE, NULL));
    // stack_ast_push(ast, ast_item_const(AST_END_BLOCK, NULL));
    // stack_ast_push(ast, ast_item_const(AST_STRING, tmpString));
    // stack_ast_push(ast, ast_item_const(AST_IF, NULL));

    // stack_ast_push(ast, ast_item_const(AST_VAR, &var));
    // stack_ast_push(ast, ast_item_const(AST_IF, NULL));


    /* function declare and return
    ht_item_t fnc_declare;
    fnc_declare.identifier = "write";

    stack_ast_push(ast, ast_item_const(AST_END_BLOCK, NULL));
    stack_ast_push(ast, ast_item_const(AST_RETURN_VOID, NULL));
    stack_ast_push(ast, ast_item_const(AST_FUNCTION_DECLARE, &fnc_declare));
    */

    return errorCode;
}
