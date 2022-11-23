#ifndef IFJ_CODE_GEN_DATA_H
#define IFJ_CODE_GEN_DATA_H

typedef enum {
    BLOCK_IF,
    BLOCK_ELSE,
    BLOCK_WHILE,
    BLOCK_DECLARE
} code_block_type;

typedef struct {
    code_block_type type;
    int labelNum;
} code_block;

#endif // IFJ_CODE_GEN_DATA_H
