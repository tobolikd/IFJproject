cmake_minimum_required(VERSION 3.14)

project(IFJ-tests)

set(PROJECT_FILES
    src/lex-analyzer.c
    src/syn-analyzer.c
    src/symtable.c
    src/ast.c
    src/stack.c
    src/code-gen.c
    src/devel.c
)

set(UNIT_TESTS
    tests/test-ast.cpp
    tests/test-lex-analyzer.cpp
    tests/test-syn-analyzer.cpp
    tests/test-code-gen.cpp
)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

add_executable(tests-all ${UNIT_TESTS})
target_link_libraries(tests-all GTest::gtest_main)
target_link_libraries(tests-all projectFiles)

include (GoogleTest)
gtest_discover_tests(tests-all)
