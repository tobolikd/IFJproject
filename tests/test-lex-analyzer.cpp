#include <gtest/gtest.h>

extern "C"
{
    #include "../src/lex-analyzer.h"
}

#include <string>
#include <cstdlib>
#include <time.h>
#include <iostream>

#include "test-lex-analyzer.hpp"

using std::make_tuple;
using std::make_pair;
using std::endl;


class testCheckProlog : public testBaseForFiles {};

TEST_P(testCheckProlog, returnValue)
{
    EXPECT_EQ(returnValue, checkProlog(tmpFile)) << "Processed input: |" << dataIn << "|" << endl;
}

INSTANTIATE_TEST_CASE_P(BASIC, testCheckProlog,
                        testing::Values(
                            make_tuple(0, "<?php"),
                            make_tuple(0, "<?php "),
                            make_tuple(0, "<?php\ndeclare()"),
                            make_tuple(1, " <?php"),
                            make_tuple(1, "<?phpp"),
                            make_tuple(1, ""),
                            make_tuple(1, "<?ph"),
                            make_tuple(1, "<"),
                            make_tuple(1, "< ?php"),
                            make_tuple(1, "<\n?php"),
                            make_tuple(1, "<<"),
                            make_tuple(1, "<?php?>")
                            )
                        );


class testGetTokenCorrect : public testBaseForTokens {};

TEST_P(testGetTokenCorrect, typeAndReturnValue)
{
    returnedToken = getToken(tmpFile, &lineNum);

    ASSERT_FALSE(returnedToken == NULL) << "RETURNED TOKEN IS NULL" << endl;
    EXPECT_EQ(expectedType, returnedToken->type) << "Incorrect token type" << endl;
    EXPECT_EQ(lineNum, returnedToken->lineNum) << "Incorrect line number" << endl;
    if (expectedData != "")
    {
        ASSERT_FALSE(returnedToken->data == NULL) << "RETURNED DATA IS NULL" << endl;
        EXPECT_EQ(expectedData, returnedToken->data) << "Incorrect lexeme data" << endl;
    }
    FAIL() << tokenInfo() << endl;
}

INSTANTIATE_TEST_SUITE_P(BASIC, testGetTokenCorrect,
                        testing::Values(
                            make_tuple(Semicolon, "", ";"),
                            make_tuple(RPar,"",")"),
                            make_tuple(LPar,"","("),
                            make_tuple(RCurl,"","}"),
                            make_tuple(LCurl,"","{"),
                            make_tuple(DotSign,"","."),
                            make_tuple(MulSign,"","*"),
                            make_tuple(MinusSign,"","-"),
                            make_tuple(PlusSign,"","+"),
                            make_tuple(Comma,"",","),
                            make_tuple(Colons,"",":"),
                            make_tuple(Backslash,"","\\"), // check
                            make_tuple(Slash,"","/"),
                            make_tuple(QuestionMark,"","?"), // check
                            make_tuple(ID,"ahoj","ahoj"),
                            make_tuple(ID,"_ahoj","_ahoj"),
                            make_tuple(DollarSign,"","$"), // check
                            make_tuple(VarID,"variable","$variable"),
                            make_tuple(StringEnd,"nejaky string","\"nejaky string\""),
                            make_tuple(Assign,"","="),
                            make_tuple(StrictEquality,"","==="),
                            make_tuple(Int, "94861320", "94861320"),
                            make_tuple(Double,"97.0","97.0"),
                            make_tuple(EuldDouble,"78.7e10","78.7e10"),
                            make_tuple(EuldDouble,"78e10","78e10"),
                            make_tuple(NotEqual,"","!=="),
                            make_tuple(GreaterEqualThanSign,"",">="),
                            make_tuple(GreaterThanSign,"",">"),
                            make_tuple(LesserEqualThanSign,"","<="),
                            make_tuple(LesserThanSign,"","<")
                            )   
                        );


                        
class testGetTokenIncorrect : public testBaseForFiles {};

TEST_P(testGetTokenIncorrect, returnValue)
{
    int lineNum;
    Token *returnedToken = getToken(tmpFile, &lineNum);
    EXPECT_TRUE(returnedToken == NULL) << "RETURNED TOKEN SHOULD BE NULL\nInput: |" << dataIn << "|\nReturned token type: " << returnedToken->type << endl;
    if (returnedToken != NULL)
    {
        if (returnedToken->data != NULL)
            FAIL() << "\nReturned data: |" << returnedToken->data << "|" << endl;
        else
            FAIL() << "\nReturned data: <NULL>" << endl;
    }
    tokenDtor(returnedToken);
}

INSTANTIATE_TEST_SUITE_P(INTERMEDIATE, testGetTokenIncorrect,
                        testing::Values(
                                make_tuple(0, "$7a"),
                                make_tuple(0, "$+"),
                                make_tuple(0, "\"not ended string\\\""),
                                make_tuple(0, "89.e"),
                                make_tuple(0, "78.5e"),
                                make_tuple(0, "87e"),
                                make_tuple(0, "\"not ended string"),
                                make_tuple(0, "?4"),
                                make_tuple(0, "?invalid"),
                                make_tuple(0, "/*unterminated comment"),
                                make_tuple(0, "/*terminated comment*/"),
                                make_tuple(0, "?function"),
                                make_tuple(0, "?true"),
                                make_tuple(0, "?_"),
                                make_tuple(0, "\\"),
                                make_tuple(0, "1e-"),
                                make_tuple(0, "1e+"),
                                make_tuple(0, "1.e+"),
                                make_tuple(0, "1.2e-")
                            )
                        );
