#include <gtest/gtest.h>

extern "C"
{
    #include "../src/lex-analyzer.h"
}

#include <string>
#include <cstdlib>
#include <time.h>

#include "test-lex-analyzer.hpp"


/* checkProlog - tests
 *
 * @brief - 
 *
 */
class testCheckProlog : public ::testing::TestWithParam<std::tuple<int, std::string>> {};

INSTANTIATE_TEST_CASE_P(BASIC, testCheckProlog,
                        testing::Values(
                            std::make_tuple(0, "<?php"),
                            std::make_tuple(0, "<?php "),
                            std::make_tuple(0, "<?php\ndeclare()"),
                            std::make_tuple(1, " <?php"),
                            std::make_tuple(1, "<?phpp"),
                            std::make_tuple(1, ""),
                            std::make_tuple(1, "<?ph"),
                            std::make_tuple(1, "<"),
                            std::make_tuple(1, "< ?php"),
                            std::make_tuple(1, "<\n?php"),
                            std::make_tuple(1, "<<"),
                            std::make_tuple(1, "<?php?>")
                            )
                        );

TEST_P(testCheckProlog, returnValue)
{
    int returnValue= std::get<0>(GetParam());
    const char *in = std::get<1>(GetParam()).data();

    FILE *tmpFile = prepTmpFile(in);
    ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file" << std::endl;
    EXPECT_EQ(returnValue, checkProlog(tmpFile)) << "Processed input: \"" << in << "\"" << std::endl;
    fclose(tmpFile);
}

class testBaseForTokens : public ::testing::TestWithParam<std::tuple<AutoState, std::string, std::string>>
{
    protected:
        AutoState expectedType;
        std::string expectedData;
        int lineNum;
        Token *returnedToken = NULL;

        std::string dataIn;

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
            fclose(tmpFile);
        }

        void TearDown() override
        {
            tokenDtor(returnedToken);
        }
};

class testGetToken : public testBaseForTokens {};

INSTANTIATE_TEST_SUITE_P(BASIC, testGetToken,
                        testing::Values(
                            std::make_tuple(Semicolon,"",";"),
                            std::make_tuple(RPar,"",")"),
                            std::make_tuple(LPar,"","("),
                            std::make_tuple(RCurl,"","}"),
                            std::make_tuple(LCurl,"","{"),
                            std::make_tuple(DotSign,"","."),
                            std::make_tuple(MulSign,"","*"),
                            std::make_tuple(MinusSign,"","-"),
                            std::make_tuple(PlusSign,"","+"),
                            std::make_tuple(Comma,"",","),
                            std::make_tuple(Colons,"",":"),
                            std::make_tuple(Backslash,"","\\"), // check
                            std::make_tuple(Slash,"","/"),
                            std::make_tuple(QuestionMark,"","?"),
                            std::make_tuple(ID,"ahoj","ahoj"),
                            std::make_tuple(DollarSign,"","$"), // check
                            std::make_tuple(VarID,"variable","$variable"),
                            std::make_tuple(StringEnd,"nejaky string","\"nejaky string\""),
                            std::make_tuple(Assign,"","="),
                            std::make_tuple(StrictEquality,"","==="),
                            std::make_tuple(Int, "94861320", "94861320"),
                            std::make_tuple(Double,"97.0","97.0"),
                            std::make_tuple(EuldDouble,"78.7e10","78.7e10"),
                            std::make_tuple(EuldDouble,"78e10","78e10"),
                            std::make_tuple(NotEqual,"","!=="),
                            std::make_tuple(GreaterEqualThanSign,"",">="),
                            std::make_tuple(GreaterThanSign,"",">"),
                            std::make_tuple(LesserEqualThanSign,"","<="),
                            std::make_tuple(LesserThanSign,"","<")
                            )   
                        );

TEST_P(testGetToken, tokenTypeAndValue)
{
    ASSERT_FALSE(returnedToken == NULL) << "RETURNED TOKEN IS NULL" << std::endl;
    EXPECT_EQ(expectedType, returnedToken->type) << "Incorrect token type\t\tInput data |" << dataIn << "|" << std::endl;
    EXPECT_EQ(lineNum, returnedToken->lineNum) << "Incorrect line number" << std::endl;
    if (expectedData != "")
    {
        ASSERT_FALSE(returnedToken->data == NULL) << "RETURNED DATA IS NULL" << std::endl;
        EXPECT_EQ(expectedData, returnedToken->data) << "Incorrect lexeme data\t\tInput data |" << dataIn << "|" << std::endl;
    }
}
