#ifndef IFJ_STACK_H
#define IFJ_STACK_H 1

#include "ast.h"
#include "lex-analyzer.h"
#include "code-gen-data.h"
#include "preced-analyzer-data.h"

#include <stdbool.h>

/* Functions and data structures for ast
 *
 * Note: name is {token, ast}
 *       type is {Token *, AST_item *}
 * Functions:
 *
 * void stack_name_init(stack_name_t *stack)
 *  - initialize stack
 *
 * void stack_name_push(stack_name_t *stack, type item)
 *  - push item to top of the stack
 *
 * void stack_name_pop(stack_name_t *stack)
 *  - remove the top item from stack
 *
 * TYPE stack_name_top(stack_name_t *stack)
 *  - return pointer to item on stack top
 *
 * bool stack_name_empty(stack_name_t *stack)
 *  - predicate whether stack is empty or not
 *
 * Structures:
 * stack_name_item_t
 *  |- data - useful data
 *  |- next - pointer to the next item
 *
 * stack_name_t
 *  |- top - pointer to the top of stack
 */

#define STACK_DECLARATION(TYPE, NAME)                           \
    typedef struct stack_##NAME##_item {                        \
        TYPE data;                                              \
        struct stack_##NAME##_item *next;                       \
        struct stack_##NAME##_item *previous;                   \
    } stack_##NAME##_item_t;                                    \
                                                                \
    typedef struct {                                            \
        stack_##NAME##_item_t *top;                             \
        stack_##NAME##_item_t *bottom;                          \
    } stack_##NAME##_t;                                         \
                                                                \
  void stack_##NAME##_init(stack_##NAME##_t *stack);            \
  void stack_##NAME##_push(stack_##NAME##_t *stack, TYPE item); \
  void stack_##NAME##_push_b(stack_##NAME##_t *stack, TYPE item);\
  void stack_##NAME##_pop(stack_##NAME##_t *stack);             \
  void stack_##NAME##_pop_b(stack_##NAME##_t *stack);           \
  TYPE stack_##NAME##_bot(stack_##NAME##_t *stack);             \
  TYPE stack_##NAME##_top(stack_##NAME##_t *stack);             \
  bool stack_##NAME##_empty(stack_##NAME##_t *stack);

STACK_DECLARATION(AST_item *, ast)
STACK_DECLARATION(Token *, token)
STACK_DECLARATION(char *, string)
STACK_DECLARATION(code_block *, code_block)
STACK_DECLARATION(PrecedItem *, precedence)

#endif // IFJ_STACK_H
