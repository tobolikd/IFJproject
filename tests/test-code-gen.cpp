#include <gtest/gtest.h>
#include "../src/code-gen.h"

TEST(linkTest, codeGen) {
    EXPECT_EQ(codeGen('c'), 'c');
}
