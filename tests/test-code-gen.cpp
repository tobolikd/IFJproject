#include <gtest/gtest.h>

extern "C" {
    #include "../src/code-gen.h"
}

TEST(linkTest, codeGen) {
    EXPECT_EQ(codeGen('c'), 'c');
}
