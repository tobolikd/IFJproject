#ifndef IFJ_ERROR_CODES_H
#define IFJ_ERROR_CODES_H 1

#include <stdio.h>

enum ifjErrCode {
    SUCCESS = 0, // no error
    LEXICAL_ERR = 1, // incorrect lexeme structure
    SYNTAX_ERR = 2, // incorrect syntax
    SEMANTIC_FUNCTION_DEFINITION_ERR = 3, // undefined function,
    SEMANTIC_RUN_PARAMETER_ERR = 4, // incorrect type/count in function call
                                    // or incorrect return value type
    SEMANTIC_VARIABLE_ERR = 5, // usage of undefined variable
    SEMANTIC_RUN_RETURN_ERR = 6, // missing or redundand return expression
    SEMANTIC_RUN_TYPE_ERR = 7, // type incompabilty in aritmetic, string,
                               // or relation expressions
    SEMANTIC_OTHER_ERR = 8, // other semantic errors
    INTERNAL_ERR = 99 // interal compiler error
};

// global variable for errCode storage
extern enum ifjErrCode errorCode;

/* print debug info using debug_print() macro
 * 0 - OFF
 * 1 - ON
 */
#define DEBUG 1

// used for compiler messages (description of error)
// WILL BE IN FINAL SOLUTION
#define debug_print(...) do { if (DEBUG == 1) fprintf(stderr, __VA_ARGS__); } while (0)

/* print logs using debug_log() macro
 * 0 - OFF
 * 1 - ON
 */
#define DEBUG_LOGS 1

// used for developmnent purposes
// will NOT be printer in final solution
#define debug_log(...) do { if (DEBUG_LOGS == 1) fprintf(stderr, __VA_ARGS__); } while (0)

// check malloc success, on fail return
#define CHECK_MALLOC(ptr) do { if (ptr == NULL) {errorCode = INTERNAL_ERR; return;} } while (0)

// check malloc success, on fail return NULL
#define CHECK_MALLOC_PTR(ptr) do { if (ptr == NULL) {errorCode = INTERNAL_ERR; return NULL;} } while (0)

#define MALLOC_ERR do{errorCode = INTERNAL_ERR; fprintf(stderr, "INTERNAL: malloc returned NULL"); } while(0)

#define THROW_ERROR(CODE, ...)                                  \
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
    }

#endif // IFJ_ERROR_CODES_H
