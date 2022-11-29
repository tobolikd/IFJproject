#include <gtest/gtest.h>

extern "C"
{
    #include "../src/preced-analyzer.c"
    #include "../src/sem-analyzer.c"
    
}

#include <string>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <array>

#include "../src/preced-analyzer.h"
#include "test-preced-analyzer.hpp"

using std::make_tuple;
using std::make_pair;
using std::endl;


class testCheckReturnValue : public testBaseAST {};


TEST_P(testCheckReturnValue, expectedValue)
{
    EXPECT_EQ(expectedValue,parseExpression(testList,&index,testTableVar,&testStack)) << "Processing input: |" << get<1>(GetParam()) << "| ..." << endl;
}

INSTANTIATE_TEST_SUITE_P(CONST_CORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(true, "2"),
        make_tuple(true, "2+2"),
        make_tuple(true, "2+2+2"),
        make_tuple(true, "(2+2)"),
        make_tuple(true, "(2+2)+2"),
        make_tuple(true, "(2)+2"),
        make_tuple(true, "-2+2"),
        make_tuple(true, "(-2)"),
        make_tuple(true, "2*2"),
        make_tuple(true, "2*2+2"),
        make_tuple(true, "2*2+2-2/2"),
        make_tuple(true, "(2)+(2)*(2)/(2)"),
        make_tuple(true, "-2+(-2)"),
        make_tuple(true, "-2*(-2)"),
        make_tuple(true, "-(+2)-(+2)")
    )
);

INSTANTIATE_TEST_SUITE_P(CONST_INCORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(false, ""),
        make_tuple(false, "()"),
        make_tuple(false, "2 2"),
        make_tuple(false, "2++"),
        make_tuple(false, ")2"),
        make_tuple(false, "(2+2)2"),
        make_tuple(false, "2(2-2)"),
        make_tuple(false, "(2+2)*2*"),
        make_tuple(false, "2(+)2"),
        make_tuple(false, "(2*2"),
        make_tuple(false, "2+-2"),
        make_tuple(false, "2*(2"),
        make_tuple(false, "2/"),
        make_tuple(false, "**2"),
        make_tuple(false, "2**(-2"),
        make_tuple(false, "(+2)(-2)")
    )
);

INSTANTIATE_TEST_SUITE_P(VARIABLES_CORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(true, "$a"),
        make_tuple(true, "(-$a))"),
        make_tuple(true, "$a+$a"),
        make_tuple(true, "$a+$a+$a"),
        make_tuple(true, "($a+$a)"),
        make_tuple(true, "($a+$a)+$a"),
        make_tuple(true, "($a)+$a"),
        make_tuple(true, "-$a+$a"),
        make_tuple(true, "(-$a)"),
        make_tuple(true, "$a*$a"),
        make_tuple(true, "$a*$a+$a"),
        make_tuple(true, "$a*$a+$a-$a/$a"),
        make_tuple(true, "($a)+($a)*($a)/($a)"),
        make_tuple(true, "-$a+(-$a)"),
        make_tuple(true, "-$a*(-$a)"),
        make_tuple(true, "-(+$a)-(+$a)")
    )
);

INSTANTIATE_TEST_SUITE_P(VARIABLES_INCORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(false, "$a $a"),
        make_tuple(false, "$a++"),
        make_tuple(false, ")$a"),
        make_tuple(false, "($a+$a)$a"),
        make_tuple(false, "$a($a-$a)"),
        make_tuple(false, "($a+$a)*$a*"),
        make_tuple(false, "$a(+)$a"),
        make_tuple(false, "($a*$a"),
        make_tuple(false, "$a+-$a"),
        make_tuple(false, "$a*($a"),
        make_tuple(false, "$a/"),
        make_tuple(false, "**$a"),
        make_tuple(false, "$a**(-$a"),
        make_tuple(false, "(+$a)(-$a)")
    )
);

INSTANTIATE_TEST_SUITE_P(LOGICAL_OP_CORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(true, "$a === $a"),
        make_tuple(true, "2 < $a"),
        make_tuple(true, "$a <= -2"),
        make_tuple(true, "(3+3)*3 < 3*(3*3)"),
        make_tuple(true, "$a . $a < 2"),
        make_tuple(true, "2 < 2 === 2"),
        make_tuple(true, "2 === 2 > 2"),
        make_tuple(true, "(2+2<2) === 1"),
        make_tuple(true, "(2+2<2) === 1 < $a"),
        make_tuple(true, "(($a === $a)===(5===5))")
    )
);

INSTANTIATE_TEST_SUITE_P(LOGICAL_OP_INCORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(false, "$a === $a === $a"),
        make_tuple(false, "$a + < 2"),
        make_tuple(false, "$a === *2"),
        make_tuple(false, "$a < $a < $a"),
        make_tuple(false, "$a <= 2 <= 2 "),
        make_tuple(false, "(($a === $a)(===)(5===5))"),
        make_tuple(false, "(2 === 2) === (2 === 2)===(5===5)"),
        make_tuple(false, "2 < $a (!== $a) "),
        make_tuple(false, "==="),
        make_tuple(false, "2 <")
    )
);

class testCheckAST : public testBaseAST {};

TEST_P(testCheckAST, expectedValue)
{
    EXPECT_EQ(expectedValue,parseExpression(testList,&index,testTableVar,&testStack)) << "Processing input: |" << get<1>(GetParam()) << "| ..." << endl;
}

INSTANTIATE_TEST_SUITE_P(FUNCTION_CALL_CORRECT, testCheckAST,
    testing::Values(
        make_tuple(true, "foo(1.2)"),
        make_tuple(true, "doe()"),
        make_tuple(true, "poo(1.2,1.2)"),
        make_tuple(true, "$a * foo(1.2) < $a"),
        make_tuple(true, "$a/foo(1.2) <= -foo(1.2)+2"),
        make_tuple(true, "(foo(1.2)+foo(1.2))*foo(1.2) < foo(1.2)*(foo(1.2)*foo(1.2))"),
        make_tuple(true, "2 < foo(1.2) === 2"),
        make_tuple(true, "2 === $a > $b"),
        make_tuple(true, "(2+2<2) === foo(1.2)>foo(1.2)"),
        make_tuple(true, "(foo(1.2)+foo(1.2)<foo(1.2)) === 1 < $a"),
        make_tuple(true, "-foo(1.2)"),
        make_tuple(true, "foo($a)"),
        make_tuple(true, "foo($b)"),
        make_tuple(true, "foo(1.2) . foo($a) < 2"),
        make_tuple(true, "((+foo(1.2) === -foo(1.2))===(5===5))")
    )
);

INSTANTIATE_TEST_SUITE_P(FUNCTION_CALL_INCORRECT, testCheckAST,
    testing::Values(
        make_tuple(false, "foo()"),
        make_tuple(false, "poo(1.2)"),
        make_tuple(false, "poo(1.2,1.2,1.2)"),
        make_tuple(false, "poo(1.2,,1.2)"),
        make_tuple(false, "poo(2,2"),
        make_tuple(false, "doe(1.2)"),
        make_tuple(false, "doe(,)"),
        make_tuple(false, "foo(\"ahoj\")"),
        make_tuple(false, "foo(1)"),
        make_tuple(false, "poo(1,1)"),
        make_tuple(false, "foo(1.2,)"),
        make_tuple(false, "foo(1.2,1.2)"),
        make_tuple(false, "foo($c)"),
        make_tuple(false, "$c"),
        make_tuple(false, "fnc(1.2) === 2"),
        make_tuple(false, "foo 1.2 < $a < $a"),
        make_tuple(false, "foo(x)"),
        make_tuple(false, "$a + $var"),
        make_tuple(false, "doo(1.2)"),
        make_tuple(false, "abc;"),
        make_tuple(false, "3+doo(1.2)"),
        make_tuple(false, "doo(1.2)"),
        make_tuple(false, "$a + foo(\"ahoj\")"),
        make_tuple(false, "$foo(1.2) . $foo(1.2) < 2"),
        make_tuple(false, "$foo(1.2) . $foo(1.2)"),
        make_tuple(false, "foo(1.2)<")
    )
);

class DoubleValue : public CheckDoubleValue {};

TEST_P(DoubleValue, expectedValue)
{
    parseExpression(testList,&index,testTableVar,&testStack);
    EXPECT_DOUBLE_EQ(expectedValue,testStack.top->data->data->floatValue) << "Processing input: |" << get<1>(GetParam()) << "| ..." << endl;
}

INSTANTIATE_TEST_SUITE_P(CONST_CORRECT_ADVANCED, DoubleValue,
    testing::Values(
        make_tuple(20, "2e1"),
        make_tuple(20, "2E1"),
        make_tuple(0.2, "2e-1"),
        make_tuple(0.2, "2E-1"),
        make_tuple(20, "2e+1"),
        make_tuple(20, "2E+1"),
        make_tuple(20, "2.0e+1"),
        make_tuple(0.2, "2.0e-1"),
        make_tuple(2, "0.00002e5"),
        make_tuple(-2, "-2.0e0"),
        make_tuple(-2, "-0.00002e5"),
        make_tuple(-21, "-2.1e1")
    )
);

