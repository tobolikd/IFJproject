#include <gtest/gtest.h>

int main() {
    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
