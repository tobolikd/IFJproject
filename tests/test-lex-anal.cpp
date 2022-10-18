#include <gtest/gtest.h>
#include "../src/lex-anal.h"

TEST(linkTest, lexAnal) {
    EXPECT_EQ(lexAnal('a'), 'a');
}
