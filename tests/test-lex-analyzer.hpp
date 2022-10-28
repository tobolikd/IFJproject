#ifndef IFJ_TEST_LEX_ANALYZER_H
#define IFJ_TEST_LEX_ANALYZER_H

extern "C"
{
    #include "../src/lex-analyzer.h"
}

#include <string>
#include <iostream>
#include <gtest/gtest.h>

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

/* test base for token tests
 *
 * prepare parametres, tmpfile, token to string,...
 *
 * to be inherited
 */
class testBaseForTokens : public ::testing::TestWithParam<std::tuple<AutoState, std::string, std::string>>
{
    protected:
        std::string dataIn;
        int lineNum;

        AutoState expectedType;
        std::string expectedData;
        
        Token *returnedToken = NULL;

        void SetUp() override
        {
            srand(time(0)); // init rand()
            
            expectedType = std::get<0>(GetParam());
            expectedData = std::get<1>(GetParam());
            dataIn = std::get<2>(GetParam()).data();
            
            lineNum = rand(); // get random line num

            FILE *tmpFile = prepTmpFile(dataIn.data());
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file" << std::endl;
            returnedToken = getToken(tmpFile, &lineNum);
            if (tmpFile != NULL)
                fclose(tmpFile);
        }

        void TearDown() override
        {
            tokenDtor(returnedToken);
        }

        std::string tokenInfo()
        {
            std::string out = "";
            out += "\nInput file: |" + dataIn + "|";
            out += "\nExpected Type: " + std::to_string(expectedType);
            out += "\nEpected Data: |" + expectedData + "|";

            if (returnedToken == NULL)
            {
                out += "\nReturned token <NULL>";
                return out;
            }
            out += "\nReturned token (" + std::to_string(reinterpret_cast<intptr_t>(returnedToken)) + ")";
            out += "\n\tType: " + std::to_string(returnedToken->type);
            std::string data = (returnedToken->data == NULL) ? "<NULL>" : "|" + std::string(returnedToken->data) + "|";
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
class testBaseForFiles : public ::testing::TestWithParam<std::tuple<int, std::string>>
{
    protected:
        int returnValue;
        const char *dataIn;

        FILE *tmpFile;

        void SetUp() override
        {
            printf("setting up");
            returnValue = std::get<0>(GetParam());
            dataIn = std::get<1>(GetParam()).data();

            printf("got values");
            tmpFile = prepTmpFile(dataIn);
            printf("got file");
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file" << std::endl;
            printf("asserted");
        }

        void TearDown() override
        {
            if (tmpFile != NULL)
                fclose(tmpFile);
        }
};
#endif // IFJ_TEST_LEX_ANALYZER_H
