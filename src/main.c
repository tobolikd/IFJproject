#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "code-gen.h"
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

    fncTable = InitializedHTableFnctionDecs(list); // first descent
    if (fncTable == NULL)                          // error in first descent
    {
        debug_log("\n Error in first descent. Error code: %i.\n", errorCode);
        listDtor(list);
        fclose(fp);
        return errorCode;
    }

#if (DEBUG == 1)
    // printTokenList(list);
#endif

    SyntaxItem *SyntaxItem = synAnalyser(list);
    debug_log("BOOL VALUE: %s\n", SyntaxItem->correct ? "true" : "false");
    if (SyntaxItem->correct == true) // start syn analyzer
    {
        debug_log("Parser ran successfully\n");
        //printAstStack(SyntaxItem->stackAST);
        codeGenerator(SyntaxItem->stackAST, SyntaxItem->table);
    }
    // free memory
    listDtor(list);
    SyntaxDtor(SyntaxItem);
    while (!stack_declare_empty(&stackDeclare))
    {
        stack_declare_pop(&stackDeclare);
    }
    fclose(fp);

    debug_log("PROGRAM RETURNED %i.\n", errorCode);
    return errorCode;
}
