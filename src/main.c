#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "code-gen.h"
#include "error-codes.h"

enum ifjErrCode errorCode = SUCCESS;

int main () {
    /* SCANNER */
    FILE *fp;

    fp = fopen("./input.php", "r");

    TokenList *list = lexAnalyser(fp); // get list of tokens

    if(list == NULL) // there was an error in lexAnalyser
    {
        fclose(fp);
        return errorCode;
    }
#if (DEBUG == 1)
    printTokenList(list);
#endif
    synAnalyser(list);  // start syn analyzer

    // free memory
    listDtor(list);
    fclose(fp);
    /* END OF SCANNER */

    printf("\nError code: %i\n", errorCode);
    return errorCode;
}
