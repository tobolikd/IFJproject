#ifndef IFJ_TEST_LEX_ANALYZER_H
#define IFJ_TEST_LEX_ANALYZER_H

extern "C"
{
    #include "../src/lex-analyzer.h"
}

#include <string>
#include <iostream>
#include <gtest/gtest.h>

using std::tuple;
using std::to_string;
using std::get;
using std::cerr;
using std::vector;
using std::pair;
using std::endl;
using std::string;
using std::array;

// returns file pointer to tmp file with given string in it
FILE *prepTmpFile(const char *in)
{
    FILE *file = tmpfile();
    
    if (file == NULL)
    {
        cerr << "\n\nTEST ERROR - could not open tmpfile\n\n";
        return NULL;
    }
    
    fputs(in, file);

    rewind(file);

    return file;
}

/* test base for token tests
 *
 * prepare parametres, tmpfile, token to string,...
 *
 * to be inherited
 */
class testBaseForTokens : public ::testing::TestWithParam<tuple<AutoState, string, string>>
{
    protected:
        string dataIn;
        int lineNum;

        AutoState expectedType;
        string expectedData;

        FILE *tmpFile;
        Token *returnedToken = NULL;

        void SetUp() override
        {
            srand(time(0)); // init rand()
            
            expectedType = get<0>(GetParam());
            expectedData = get<1>(GetParam());
            dataIn = get<2>(GetParam());
            
            lineNum = rand(); // get random line num

            tmpFile = prepTmpFile(dataIn.data());
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file" << endl;
        }

        void TearDown() override
        {
            if (tmpFile != NULL)
                fclose(tmpFile);

            tokenDtor(returnedToken);
        }

        string tokenInfo()
        {
            string out = "";
            out += "\nInput file: |" + dataIn + "|";
            out += "\nExpected Type: " + to_string(expectedType);
            out += "\nExpected Data: |" + expectedData + "|";

            if (returnedToken == NULL)
            {
                out += "\nReturned token <NULL>";
                return out;
            }
            out += "\nReturned token (" + to_string(reinterpret_cast<intptr_t>(returnedToken)) + ")";
            out += "\n\tType: " + to_string(returnedToken->type);
            string data = (returnedToken->data == NULL) ? "<NULL>" : "|" + string(returnedToken->data) + "|";
            out += "\n\tData: " + data;
            return out; 
        }
};

/* test base for tests which need temp file
 *
 * open, input data and close tmpFile
 *
 * to be inherited
 */
class testBaseForFiles : public ::testing::TestWithParam<tuple<int, string>>
{
    protected:
        int returnValue;
        const char *dataIn;

        FILE *tmpFile;

        void SetUp() override
        {
            printf("setting up");
            returnValue = get<0>(GetParam());
            dataIn = get<1>(GetParam()).data();

            printf("got values");
            tmpFile = prepTmpFile(dataIn);
            printf("got file");
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file" << endl;
            printf("asserted");
        }

        void TearDown() override
        {
            if (tmpFile != NULL)
                fclose(tmpFile);
        }
};
#endif // IFJ_TEST_LEX_ANALYZER_H