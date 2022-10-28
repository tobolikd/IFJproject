#ifndef IFJ_ERROR_CODES_H
#define IFJ_ERROR_CODES_H

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
// enum ifjErrCode errorCode = SUCCESS;

// define 1 for debug logs, 0 for none
#define DEBUG 1

#define debug_print(...) \
            do { if (DEBUG == 1) fprintf(stderr, __VA_ARGS__); } while (0)

#endif // IFJ_ERROR_CODES_H
