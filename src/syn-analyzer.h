#ifndef IFJ_SYN_ANALYZER_H
#define IFJ_SYN_ANALYZER_H

#include <stdbool.h>

typedef enum
{
    void_t,
    int_t,
    string_t,
    float_t,
    null_int_t,
    null_string_t,
    null_float_t
} VarType;

typedef struct
{
    VarType type;
    char *varId;
} VarInfo;

typedef struct
{
    char *functionId;
    int paramCount;
    VarInfo *params;
    VarType returnType;
} FunctionInfo;

bool isIntegrated(char *functionId);

#endif // IFJ_SYN_ANALYZER_H
