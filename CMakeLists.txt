cmake_minimum_required(VERSION 3.14)

project(IFJ-tests)

set(PROJECT_FILES
    src/lex-anal.c
    src/syn-anal.c
    src/code-gen.c
)

set(UNIT_TESTS
    tests/test-lex-anal.cpp
    tests/test-syn-anal.cpp
    tests/test-code-gen.cpp
    tests/tests-main.cpp
)


set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(projectFiles ${PROJECT_FILES})

add_library(proj1 src/lex-anal.c)

add_executable(tests-all ${UNIT_TESTS})
target_link_libraries(tests-all GTest::gtest GTest::gtest_main)
target_link_libraries(tests-all projectFiles)

include (GoogleTest)
gtest_discover_tests(tests-all)
