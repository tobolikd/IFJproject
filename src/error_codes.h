/**
 * IFJ22 compiler
 *
 * @file devel.h
 * 
 * @brief error/debug management library
 *
 * @author Gabriel Biel (xbielg00),
 * @author David Tobolik (xtobol06)
 */

#ifndef IFJ_ERROR_CODES_H
#define IFJ_ERROR_CODES_H 1

#include <stdio.h>
#include "devel.h"

enum ifjErrCode {
    SUCCESS = 0, // no error
    LEXICAL_ERR = 1, // incorrect lexeme structure
    SYNTAX_ERR = 2, // incorrect syntax
    SEMANTIC_FUNCTION_DEFINITION_ERR = 3, // undefined function, function redefintion
    SEMANTIC_PARAMETER_ERR = 4,
    SEMANTIC_RUN_PARAMETER_ERR = 4, // incorrect type/count in function call
                                    // or incorrect return value type
    SEMANTIC_VARIABLE_ERR = 5, // usage of undefined variable
    SEMANTIC_RUN_VARIABLE_ERR = 5, // usage of undefined variable
    SEMANTIC_RETURN_ERR = 6,
    SEMANTIC_RUN_RETURN_ERR = 6, // missing or excess return expression
    SEMANTIC_RUN_TYPE_ERR = 7, // type incompabilty in aritmetic, string,
                               // or relation expressions
    SEMANTIC_OTHER_ERR = 8, // other semantic errors
    INTERNAL_ERR = 99 // interal compiler error
};

// global variable for errCode storage
extern enum ifjErrCode errorCode;

// turn on/off compiler info to stderr
#define COMPILER_INFO 1

#if COMPILER_INFO == 1
    // used for compiler messages (description of error)
    // WILL BE IN FINAL SOLUTION
    #define debug_print(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug_print(...)
#endif

/* print logs using debug_log() macro and some other developer functions in code
 * 0 - OFF
 * 1 - ON
 */
#define DEBUG 0

#if DEBUG == 1
    // used for developmnent purposes
    // will NOT be printed in final solution
    #define debug_log(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug_log(...)
#endif

// check malloc success, on fail return
#define CHECK_MALLOC(ptr) do { if (ptr == NULL) {errorCode = INTERNAL_ERR; return;} } while (0)

// check malloc success, on fail return NULL
#define CHECK_MALLOC_PTR(ptr) do { if (ptr == NULL) {errorCode = INTERNAL_ERR; return NULL;} } while (0)

// set err code, print message, but dont return
#define MALLOC_ERR() do{ errorCode = INTERNAL_ERR; fprintf(stderr, "INTERNAL: malloc returned NULL"); } while(0)

#define THROW_ERROR(CODE, ...)                                      \
    if (errorCode == SUCCESS){                                      \
        errorCode = CODE;                                           \
        switch (errorCode){                                         \
        case LEXICAL_ERR:                                           \
            debug_print("LEXICAL ERROR on line number: %d\n",__VA_ARGS__); \
            break;                                                  \
        case SYNTAX_ERR:                                            \
            debug_print("SYNTAX ERROR on line number: %d\n",__VA_ARGS__);  \
            break;                                                  \
        case SEMANTIC_FUNCTION_DEFINITION_ERR:                      \
            debug_print("SEMANTIC FUNCTION DEFINITION ERROR on line number: %d\n",__VA_ARGS__);\
            break;                                                  \
        case SEMANTIC_RUN_PARAMETER_ERR:                            \
            debug_print("SEMANTIC RUN PARAMETER ERROR on line number: %d\n",__VA_ARGS__);\
            break;                                                  \
        case SEMANTIC_VARIABLE_ERR:                                 \
            debug_print("SEMANTIC VARIABLE ERROR on line number: %d\n",__VA_ARGS__);\
            break;                                                  \
        case SEMANTIC_RUN_RETURN_ERR:                               \
            debug_print("SEMANTIC RUN RETURN ERROR on line number: %d\n",__VA_ARGS__);\
            break;                                                  \
        case SEMANTIC_RUN_TYPE_ERR:                                 \
            debug_print("SEMANTIC RUN TYPE ERROR on line number: %d\n",__VA_ARGS__);\
            break;                                                  \
        case SEMANTIC_OTHER_ERR:                                    \
            debug_print("SEMANTIC OTHER ERROR on line number: %d\n",__VA_ARGS__);\
            break;                                                  \
        case INTERNAL_ERR:                                          \
            debug_print("INTERNAL ERROR ");                         \
            break;                                                  \
        default:                                                    \
            printf("UNKNOWN ERROR. ADD TO LIST? \n");               \
    } }

/// MACROS FOR COMPILER ERRORS

// undeclared function
#define ERR_FNC_NOT_DECLARED(ID)                                    \
do {errorCode = SEMANTIC_FUNCTION_DEFINITION_ERR;                   \
    debug_print("ERROR: calling undefined function: %s\n", ID);     \
} while (0)

// function param type error
#define ERR_FNC_PARAM_TYPE(FNC)                                                     \
do {errorCode = SEMANTIC_PARAMETER_ERR;                                   \
    debug_print("ERROR in function call: %s\n\tincompatible function call parameter type\n", FNC);  \
} while (0)

// function param count error
#define ERR_FNC_PARAM_COUNT(FNC)                                                \
do {errorCode = SEMANTIC_PARAMETER_ERR;                                         \
    debug_print("ERROR in function call: %s\n\twrong parameter count\n", FNC);  \
} while (0)

// internal error in compiler
#define ERR_INTERNAL(FUNCTION, ... ) do { errorCode = INTERNAL_ERR;        \
            debug_print("ERROR(internal):\n\tin function: %s\n\t", #FUNCTION);    \
            debug_print(__VA_ARGS__);                                      \
            } while (0)

// internal - not empty ast
#define ERR_AST_NOT_EMPTY(AST)                                    \
do {errorCode = INTERNAL_ERR;                      \
    debug_print("ERROR(internal): in function codeGenerator - ast stack not empty");     \
    print_ast_stack(AST);\
} while (0)

#endif // IFJ_ERROR_CODES_H
