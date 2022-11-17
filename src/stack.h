#ifndef IFJ_STACK_H
#define IFJ_STACK_H

#include "ast.h"
#include "lex-analyzer.h"

#include <stdbool.h>

#define STACK_DECLARATION(TYPE, NAME)                           \
    typedef struct stack_##NAME##_item {                        \
        TYPE data;                                              \
        struct stack_##NAME##_item *next;                       \
    } stack_##NAME##_item_t;                                    \
                                                                \
    typedef struct {                                            \
        stack_##NAME##_item_t *top;                             \
    } stack_##NAME##_t;                                         \
                                                                \
  void stack_##NAME##_init(stack_##NAME##_t *stack);            \
  void stack_##NAME##_push(stack_##NAME##_t *stack, TYPE item); \
  void stack_##NAME##_pop(stack_##NAME##_t *stack);             \
  TYPE stack_##NAME##_top(stack_##NAME##_t *stack);             \
  bool stack_##NAME##_empty(stack_##NAME##_t *stack);

STACK_DECLARATION(AST_item *, ast)
STACK_DECLARATION(Token *, token)

#endif // IFJ_STACK_H
