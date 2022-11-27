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
            returnValue = get<0>(GetParam());
            dataIn = get<1>(GetParam()).data();

            tmpFile = prepTmpFile(dataIn);
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file" << endl;
        }

        void TearDown() override
        {
            if (tmpFile != NULL)
                fclose(tmpFile);
        }
};

/* test base for token tests
 *
 * prepare parametres, tmpfile, token to string,...
 *
 * to be inherited
 */
class testBaseForTokens : public ::testing::TestWithParam<tuple<TokenType, string, string>>
{
    protected:
        string dataIn;
        int lineNum;

        TokenType expectedType;
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
            string out = "\n";
            out += "\nInput file: |" + dataIn + "|";
            out += "\nExpected Type: " + string(TOKEN_TYPE_STRING[expectedType]);
            out += "\nExpected Data: |" + expectedData + "|";

            if (returnedToken == NULL)
            {
                out += "\n\nReturned token <NULL>";
                return out;
            }
            out += "\n\nReturned token (" + to_string(reinterpret_cast<intptr_t>(returnedToken)) + ")";
            out += "\n\tType: " + string(TOKEN_TYPE_STRING[returnedToken->type]);
            string data = (returnedToken->data == NULL) ? "<NULL>" : "|" + string(returnedToken->data) + "|";
            out += "\n\tData: " + data;
            out += "\n\tLine number: " + to_string(lineNum);
            return out;
        }
};

/* test base for lex analyzer
 *
 * prepare parametres, tmpfile, token to string,...
 *
 * to be inherited
 */
class testBaseForLex : public ::testing::TestWithParam<tuple<array<TokenType, 5>, array<string, 5>, string>>
{
    protected:
        string dataIn;
        int lineNum;

        array<TokenType, 5> expectedType;
        array<string, 5> expectedData;

        FILE *tmpFile;
        Token *returnedToken = NULL;
        TokenList *returnedList = NULL;
        int tokenNum = 0;

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
        }

        string tokenInfo(int i)
        {
            string out = "\n";
            out += "\nExpected Type[" + to_string(i) + "]: " + string(TOKEN_TYPE_STRING[expectedType[i]]);
            out += "\nExpected Data[" + to_string(i) + "]: |" + expectedData[i] + "|";

            if (returnedToken == NULL)
            {
                out += "\n\nReturned token <NULL>";
                return out;
            }
            out += "\n\nReturned token [" + to_string(i) + "] (" + to_string(reinterpret_cast<intptr_t>(returnedToken)) + ")";
            out += "\n\tType: " + string(TOKEN_TYPE_STRING[returnedToken->type]);
            out += "\n\tData: ";
            if (returnedToken->data == NULL) {out += "<NULL>";} else {out += "|" + string(returnedToken->data) + "|";}
            out += "\n\tLine number: " + to_string(lineNum) + "\n";
            return out;
        }

        string listInfo()
        {
            string out = "\n";
            out += "\nInput file: |" + dataIn + "|\n";
            for (int i = 0; i < returnedList->length; i++)
            {
                returnedToken = returnedList->TokenArray[i];
                out += tokenInfo(i);
            }
            return out;
        }

        string listInfo(int lastCorrect)
        {
            string out = "\nList of correct tokens\n";
            out += "\nInput file: |" + dataIn + "|\n";
            for (int i = 0; i <= lastCorrect; i++)
            {
                returnedToken = returnedList->TokenArray[i];
                out += tokenInfo(i);
            }
            return out;
        }
};

#endif // IFJ_TEST_LEX_ANALYZER_H
