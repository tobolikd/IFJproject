#ifndef IFJ_TEST_LEX_ANALYZER_H
#define IFJ_TEST_LEX_ANALYZER_H

#include <string>
#include <iostream>


// returns file pointer to tmp file with given string in it
FILE *prepTmpFile(const char *in)
{
    FILE *file = tmpfile();
    
    if (file == NULL)
    {
        std::cerr << "\n\nTEST ERROR - could not open tmpfile\n\n";
        return NULL;
    }
    
    fputs(in, file);

    rewind(file);

    return file;
}


#endif // IFJ_TEST_LEX_ANALYZER_H
