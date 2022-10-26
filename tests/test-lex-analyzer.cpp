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


class testCheckProlog : public testBaseForFiles {};

TEST_P(testCheckProlog, returnValue)
{
    EXPECT_EQ(returnValue, checkProlog(tmpFile)) << "Processed input: |" << dataIn << "|" << std::endl;
}

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


class testGetTokenCorrect : public testBaseForTokens {};

TEST_P(testGetTokenCorrect, typeAndReturnValue)
{
    ASSERT_FALSE(returnedToken == NULL) << "RETURNED TOKEN IS NULL" << std::endl;
    EXPECT_EQ(expectedType, returnedToken->type) << "Incorrect token type\n" << std::endl;
    EXPECT_EQ(lineNum, returnedToken->lineNum) << "Incorrect line number" << std::endl;
    if (expectedData != "")
    {
        ASSERT_FALSE(returnedToken->data == NULL) << "RETURNED DATA IS NULL" << std::endl;
        EXPECT_EQ(expectedData, returnedToken->data) << "Incorrect lexeme data\n" << std::endl;
    }
    FAIL() << tokenInfo() << std::endl;
}

INSTANTIATE_TEST_SUITE_P(BASIC, testGetTokenCorrect,
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
                            std::make_tuple(QuestionMark,"","?"), // check
                            std::make_tuple(ID,"ahoj","ahoj"),
                            std::make_tuple(ID,"_ahoj","_ahoj"),
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


                        
class testGetTokenIncorrect : public testBaseForFiles {};

TEST_P(testGetTokenIncorrect, returnValue)
{
    int lineNum;
    Token *returnedToken = getToken(tmpFile, &lineNum);
    EXPECT_TRUE(returnedToken == NULL) << "RETURNED TOKEN SHOULD BE NULL\nInput: |" << dataIn << "|\nReturned token type: " << returnedToken->type << std::endl;
    tokenDtor(returnedToken);
}

INSTANTIATE_TEST_SUITE_P(INTERMEDIATE, testGetTokenIncorrect,
                        testing::Values(
                                std::make_tuple(0, "$7a"),
                                std::make_tuple(0, "$+"),
                                std::make_tuple(0, "\"not ended string\\\""),
                                std::make_tuple(0, "89.e"),
                                std::make_tuple(0, "78.5e"),
                                std::make_tuple(0, "87e"),
                                std::make_tuple(0, "\"not ended string"),
                                std::make_tuple(0, "?4"),
                                std::make_tuple(0, "?invalid"),
                                std::make_tuple(0, "/*unterminated comment"),
                                std::make_tuple(0, "/*terminated comment*/"),
                                std::make_tuple(0, "?function"),
                                std::make_tuple(0, "?true"),
                                std::make_tuple(0, "?_"),
                                std::make_tuple(0, "\\"),
                                std::make_tuple(0, "1e-"),
                                std::make_tuple(0, "1e+"),
                                std::make_tuple(0, "1.e+"),
                                std::make_tuple(0, "1.2e-")
                            )
                        );
