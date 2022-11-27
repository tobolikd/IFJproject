#include <gtest/gtest.h>

extern "C"
{
    #include "../src/lex-analyzer.h"
}

#include <string>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <array>

#include "test-lex-analyzer.hpp"

using std::make_tuple;
using std::make_pair;
using std::endl;


class testCheckProlog : public testBaseForFiles {};

TEST_P(testCheckProlog, returnValue)
{
    int lineNum=0;
    EXPECT_EQ(returnValue, checkProlog(tmpFile,&lineNum)) << "Processed input: |" << dataIn << "|" << endl;
}

INSTANTIATE_TEST_SUITE_P(BASIC, testCheckProlog,
                        testing::Values(
                            make_tuple(0, "<?php declare(strict_types=1);"),
                            make_tuple(0, "<?php\ndeclare(strict_types=1);"),
                            make_tuple(0, "<?php\tdeclare\t(\t\n\nstrict_types=1);"),
                            make_tuple(0, "<?php declare(strict_types=1)\n\n;"),
                            make_tuple(0, "<?php declare(strict_types=1);smthg after"),
                            make_tuple(0, "<?php declare(strict_types=1);?>"),
                            make_tuple(0, "<?php//myLineCommen\n declare(strict_types=1);"),
                            make_tuple(0, "<?php/* myStarComment */ declare(strict_types=1);"),
                            make_tuple(1, "<?php\ndeclare(declare(strict_types=1);"),
                            make_tuple(1, " <?phpdeclare(strict_types=1);"),
                            make_tuple(1, ""),
                            make_tuple(1, "<?php//declare(strict_types=1);"),
                            make_tuple(1, "<?php/* declare(strict_types=1); */"),
                            make_tuple(1, "declare(strict_types=1);"),
                            make_tuple(1, "<?php declare(strict_types=2);"),
                            make_tuple(1, "<"),
                            make_tuple(1, "< ?php"),
                            make_tuple(1, "<?php declare(strict_types=1)"),
                            make_tuple(1, "<?php declare{strict_types=1}"),
                            make_tuple(1, "<\n?php"),
                            make_tuple(1, "<<")
                            )
                        );


class testGetTokenCorrect : public testBaseForTokens {};

TEST_P(testGetTokenCorrect, typeAndReturnValue)
{
    returnedToken = getToken(tmpFile, &lineNum);

    ASSERT_FALSE(returnedToken == NULL) << "RETURNED TOKEN IS NULL" << endl;
    EXPECT_EQ(expectedType, returnedToken->type) << "Incorrect token type" << tokenInfo() << endl;
    EXPECT_EQ(lineNum, returnedToken->lineNum) << "Incorrect line number" << endl;
    if (expectedData != "")
    {
        ASSERT_FALSE(returnedToken->data == NULL) << "RETURNED DATA IS NULL" << endl;
        EXPECT_EQ(expectedData, returnedToken->data) << "Incorrect lexeme data\n" << tokenInfo() << endl;
    }
}

INSTANTIATE_TEST_SUITE_P(BASIC, testGetTokenCorrect,
                        testing::Values(
                            make_tuple(t_EOF, "", ""),
                            make_tuple(t_EOF, "", "// nejaky koment"),
                            make_tuple(t_EOF, "", "//"),
                            make_tuple(t_EOF, "", "/* nejaky koment*/"),
                            make_tuple(t_EOF, "", "// "),
                            make_tuple(t_semicolon, "", ";"),
                            make_tuple(t_rPar,"",")"),
                            make_tuple(t_lPar,"","("),
                            make_tuple(t_rCurl,"","}"),
                            make_tuple(t_lCurl,"","{"),
                            make_tuple(t_operator,".","."),
                            make_tuple(t_operator,"*","*"),
                            make_tuple(t_operator,"-","-"),
                            make_tuple(t_operator,"+","+"),
                            make_tuple(t_operator,"/","/"),
                            make_tuple(t_comma,"",","),
                            make_tuple(t_colon,"",":"),
                            make_tuple(t_nullType,"string","?string"),
                            make_tuple(t_nullType,"int","?int"),
                            make_tuple(t_nullType,"float","?float"),
                            make_tuple(t_functionId,"ahoj","ahoj"),
                            make_tuple(t_functionId,"_ahoj","_ahoj"),
                            make_tuple(t_varId,"variable","$variable"),
                            make_tuple(t_varId,"variable123","$variable123"),
                            make_tuple(t_varId,"_v_a1_2__3","$_v_a1_2__3"),
                            make_tuple(t_string,"nejaky string","\"nejaky string\""),
                            make_tuple(t_string,"nejaky\045 \%d \%a string","\"nejaky\\045 \%d \%a string\""),
                            make_tuple(t_string,"ah\x01oj","\"ah\\x01oj\""),
                            make_tuple(t_string,"ah\xffoj","\"ah\\xFfoj\""),
                            make_tuple(t_string,"ah\001oj","\"ah\\001oj\""),
                            make_tuple(t_string,"ah\377oj","\"ah\\377oj\""),
                            make_tuple(t_string,"ah\3777oj","\"ah\\3777oj\""),
                            make_tuple(t_string,"ah\10021","\"ah\\10021\""),
                            make_tuple(t_string,"", "\"\""),
                            make_tuple(t_assign,"","="),
                            make_tuple(t_comparator,"===","==="),
                            make_tuple(t_comparator,"!==","!=="),
                            make_tuple(t_comparator,">=",">="),
                            make_tuple(t_comparator,">",">"),
                            make_tuple(t_comparator,"<=","<="),
                            make_tuple(t_comparator,"<","<"),
                            make_tuple(t_int, "94861320", "94861320"),
                            make_tuple(t_int, "94861320", "94861320a"),
                            make_tuple(t_int, "1", "1"),
                            make_tuple(t_float,"97.0","97.0"),
                            make_tuple(t_float,"78.7e10","78.7e10"),
                            make_tuple(t_float,"78e10","78e10"),
                            make_tuple(t_if,"","if"),
                            make_tuple(t_while,"","while"),
                            make_tuple(t_null,"","null"),
                            make_tuple(t_else,"","else"),
                            make_tuple(t_return,"","return"),
                            make_tuple(t_function,"","function"),
                            make_tuple(t_type,"int","int"),
                            make_tuple(t_type,"string","string"),
                            make_tuple(t_type,"float","float"),
                            make_tuple(t_type,"void","void"),
                            make_tuple(t_nullType,"string","?string"),
                            make_tuple(t_nullType,"int","?int"),
                            make_tuple(t_nullType,"float","?float")
                            )
                        );



class testGetTokenIncorrect : public testBaseForFiles {};

TEST_P(testGetTokenIncorrect, returnValue)
{
    int lineNum = 0;
    Token *returnedToken = getToken(tmpFile, &lineNum);
    EXPECT_TRUE(returnedToken == NULL) << "RETURNED TOKEN SHOULD BE NULL\nInput: |" << dataIn << "|\nReturned token type: " << TOKEN_TYPE_STRING[returnedToken->type] << endl;
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
                                make_tuple(0, "$"),
                                make_tuple(0, "?"),
                                make_tuple(0, "?abc"),
                                make_tuple(0, "?7"),
                                make_tuple(0, "\\"),
                                make_tuple(0, "$7a"),
                                make_tuple(0, "$+"),
                                make_tuple(0, "\"not ended string\\\""),
                                make_tuple(0, "\"ah\\x00oj\""),
                                make_tuple(0, "\"ah\\xGFoj\""),
                                make_tuple(0, "\"ah\\000oj\""),
                                make_tuple(0, "\"ah\\378oj\""),
                                make_tuple(0, "89.e"),
                                make_tuple(0, "78.5e"),
                                make_tuple(0, "87e"),
                                make_tuple(0, "\"not ended string"),
                                make_tuple(0, "?4"),
                                make_tuple(0, "?invalid"),
                                make_tuple(0, "/*unterminated comment"),
                                make_tuple(0, "?function"),
                                make_tuple(0, "?true"),
                                make_tuple(0, "?_"),
                                make_tuple(0, "\\"),
                                make_tuple(0, "1e-"),
                                make_tuple(0, "1e+"),
                                make_tuple(0, "?\na"),
                                make_tuple(0, "1.e+"),
                                make_tuple(0, "1.2e-")
                            )
                        );

class testLexAnalyzerCorrect : public testBaseForLex {};

TEST_P(testLexAnalyzerCorrect, tokenArray)
{
    tokenNum = 0;

    returnedList = lexAnalyser(tmpFile);

    ASSERT_FALSE(returnedList == NULL) << "RETURNED LIST IS NULL\nInput file: |" << get<2>(GetParam()) << "|" << endl;

    while (tokenNum < 5)
    {
        if (tokenNum >= returnedList->length)
        {
            FAIL() << "TOKEN LIST IS SHORTER THAN EXPECTED" << listInfo() << endl;
            break;
        }

        EXPECT_EQ(expectedType[tokenNum], returnedList->TokenArray[tokenNum]->type) << listInfo(tokenNum - 1) << "\nInvalid token type\nExpected: " << TOKEN_TYPE_STRING[expectedType[tokenNum]] << "\nGot: |" << TOKEN_TYPE_STRING[returnedList->TokenArray[tokenNum]->type] << "|" << endl;
        if (expectedData[tokenNum] != "")
        {
            ASSERT_FALSE(returnedList->TokenArray[tokenNum]->data == NULL) << listInfo(tokenNum - 1) << "\nINCORRECT TOKEN:" << tokenInfo(tokenNum) << endl;
            EXPECT_EQ(expectedData[tokenNum], returnedList->TokenArray[tokenNum]->data) << listInfo(tokenNum - 1) << "\nInvalid data\nExpected: |" << expectedData[tokenNum] << "|\nGot: " << returnedList->TokenArray[tokenNum]->data << endl;
        }

        tokenNum++;
    }

    listDtor(returnedList);
}

INSTANTIATE_TEST_SUITE_P(ADVANCED, testLexAnalyzerCorrect,
                        testing::Values(
                            make_tuple(
                                array<TokenType, 5>{t_functionId, t_lPar, t_functionId, t_assign, t_int},
                                array<string, 5>{"declare","","strict_types","","1"},
                                "<?php declare(strict_types=1);declare(strict_types=1);"),//
                            make_tuple(
                                array<TokenType, 5>{t_varId, t_assign, t_int, t_semicolon, t_EOF},
                                array<string, 5>{"int","","5","",""},
                                "<?php\tdeclare(strict_types=1);\t$int=5; // nejaky koment"),
                            make_tuple(
                                array<TokenType, 5>{t_functionId, t_lPar, t_varId, t_rPar, t_semicolon},
                                array<string, 5>{"func","","int","",""},
                                "<?php\ndeclare(strict_types=1);func\t($int);"),
                            make_tuple(
                                array<TokenType, 5>{t_varId, t_assign, t_int, t_operator, t_varId},
                                array<string, 5>{"null","","9","+","_456a"},
                                "<?php declare(strict_types=1);$null\t=9+$_456a"),
                            make_tuple(
                                array<TokenType, 5>{t_if, t_nullType, t_comparator, t_null, t_colon},
                                array<string, 5>{"","string","===","",""},
                                "<?php declare(strict_types=1);if?string===null:"),
                            make_tuple(
                                array<TokenType, 5>{t_while, t_lPar, t_int, t_comparator, t_lCurl},
                                array<string, 5>{"","","5",">=",""},
                                "<?php\ndeclare(strict_types=1); while(5>={"),
                            make_tuple(
                                array<TokenType, 5>{t_rCurl, t_operator, t_string, t_nullType, t_varId},
                                array<string, 5>{"",".","\x4A\"$\043\"\tahojstring\n","float","string"},
                                "<?php declare(strict_types=1); }.\t\"\\x4A\\\"\\$\\043\\\"\\tahojstring\\n\"\t\n?float$string"),
                            make_tuple(
                                array<TokenType, 5>{t_semicolon, t_semicolon, t_lCurl, t_rCurl, t_lPar},
                                array<string, 5>{"","","","",""},
                                "<?php/* ano */declare(strict_types=1); ;;{}()"),
                            make_tuple(
                                array<TokenType, 5>{t_function, t_functionId, t_lPar, t_rPar, t_colon},
                                array<string, 5>{"","_my0fnc","","",""},
                                "<?php declare(strict_types=1);function\t_my0fnc\t()\n:\t"),
                            make_tuple(
                                array<TokenType, 5>{t_varId, t_functionId, t_colon, t_null, t_EOF},
                                array<string, 5>{"ah","oj","","",""},
                                "<?php declare( strict_types=  1) \n;$ah\noj:null\t\n"),
                            make_tuple(
                                array<TokenType, 5>{t_string, t_comparator, t_comparator, t_varId, t_float},
                                array<string, 5>{" ","<","<","var","456.7e123"},
                                "<?php declare(     strict_types\n=1);\" \"<<$var\t456.7e123")
                            )
                        );
