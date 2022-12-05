#include "lex_analyzer.h"
#include "syn-analyzer.h"
#include "code_gen.h"
#include "error-codes.h"
#include "sem-analyzer.h"
#include "devel.h"

ht_table_t *fncTable;         // Global variable - symtable for semantic controls with function declares
stack_declare_t stackDeclare; // Global variable - stack for Function Frames

int main()
{
    errorCode = SUCCESS;
    /* SCANNER */
    FILE *fp;
    fp = stdin;

    TokenList *list = lexAnalyser(fp); // get list of tokens
    if (list == NULL)                  // there was an error in lexAnalyser
    {
        fclose(fp);
        return errorCode;
    }
    // no more reading wil be done
    fclose(fp);

    // get all function declarations
    fncTable = fncDeclarationTable(list);

    if (fncTable == NULL) {
        // error in function definitions
        listDtor(list);
        ht_deleteAll(fncTable);
        return errorCode;
    }

    // syntax analysis
    SyntaxItem *SyntaxItem = synAnalyser(list);

    if (SyntaxItem->correct == true) {
        // syntax is correct, generate code
        codeGenerator(SyntaxItem->stackAST, SyntaxItem->table);
    }

    // free tokens
    listDtor(list);

    // free syntax context
    SyntaxDtor(SyntaxItem);

    // free symtable stack
    while (!stack_declare_empty(&stackDeclare)) {
        stack_declare_pop(&stackDeclare);
    }

    // free function symtable
    ht_deleteAll(fncTable);

    return errorCode;
}
