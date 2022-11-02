#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "error-codes.h"

VarInfo strlenParams[] = {{string_t, "s"}};
VarInfo substringParams[] = {{string_t, "s"},{int_t, "i"},{int_t, "j"}};
VarInfo ordParams[] = {{string_t, "c"}};
VarInfo chrParams[] = {{int_t, "i"}};

const FunctionInfo integratedFunctions[] =
{
    {"reads", 0, NULL, null_string_t},
    {"readi", 0, NULL, null_int_t},
    {"readf", 0, NULL, null_float_t},
    {"write", -1, NULL, void_t},
    {"floatval", 1, NULL, float_t},
    {"intval", 1, NULL, int_t},
    {"strval", 1, NULL, string_t},
    {"strlen", 1, strlenParams, int_t},
    {"substring", 3, substringParams, null_string_t},
    {"ord", 0, ordParams, int_t},
    {"chr", 0, chrParams, string_t}
};
