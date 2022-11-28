/* @file stack.c
 *
 * @brief implementation of stacks
 *
 * @author David Tobolik (xtobol06)
 */

#include "stack.h"
#include "error-codes.h"

#include "lex-analyzer.h"
#include "ast.h"

#include <stdlib.h>

#define STACK_DEFINITION(TYPE, NAME, DESTRUCTOR)                                \
    void stack_##NAME##_init(stack_##NAME##_t *stack) {                         \
        stack->top = NULL;                                                      \
        stack->bottom = NULL;                                                   \
    }                                                                           \
                                                                                \
    void stack_##NAME##_push(stack_##NAME##_t *stack, TYPE item) {              \
        stack_##NAME##_item_t *new = (stack_##NAME##_item_t *) malloc(sizeof(stack_##NAME##_item_t));\
        CHECK_MALLOC(new);                                                      \
        new->data = item;                                                       \
        new->next = stack->top;                                                 \
        new->previous = NULL;                                                   \
        if (stack->top == NULL)                                                 \
            stack->bottom = new;                                                \
        else                                                                    \
            stack->top->previous = new;                                         \
        stack->top = new;                                                       \
    }                                                                           \
                                                                                \
    void stack_##NAME##_pop(stack_##NAME##_t *stack) {                          \
        stack_##NAME##_item_t *deleted = stack->top;                            \
        if (deleted == NULL)                                                    \
            return;                                                             \
                                                                                \
        stack->top = deleted->next;                                             \
        if (stack->top != NULL)                                                 \
            stack->top->previous = NULL;                                        \
        DESTRUCTOR(deleted->data);                                              \
        free(deleted);                                                          \
    }                                                                           \
                                                                                \
    void stack_##NAME##_push_b(stack_##NAME##_t *stack, TYPE item) {            \
        stack_##NAME##_item_t *new = (stack_##NAME##_item_t *) malloc(sizeof(stack_##NAME##_item_t));\
        CHECK_MALLOC(new);                                                      \
        new->data = item;                                                       \
        new->next = NULL;                                                       \
        new->previous = stack->bottom;                                          \
        if (stack->top == NULL)                                                 \
            stack->top = new;                                                   \
        else                                                                    \
            stack->bottom->next = new;                                          \
        stack->bottom = new;                                                    \
    }                                                                           \
                                                                                \
    TYPE stack_##NAME##_top(stack_##NAME##_t *stack) {                          \
        if (stack->top == NULL) { return NULL; }                                \
        return stack->top->data;                                                \
    }                                                                           \
                                                                                \
    void stack_##NAME##_pop_b(stack_##NAME##_t *stack) {                        \
        stack_##NAME##_item_t *deleted = stack->bottom;                         \
        if (deleted == NULL)                                                    \
            return;                                                             \
                                                                                \
        stack->bottom = deleted->previous;                                      \
        stack->bottom->next = NULL;                                             \
        DESTRUCTOR(deleted->data);                                              \
        free(deleted);                                                          \
    }                                                                           \
                                                                                \
    TYPE stack_##NAME##_bot(stack_##NAME##_t *stack) {                          \
        if (stack->bottom == NULL) { return NULL; }                             \
        return stack->bottom->data;                                             \
    }                                                                           \
                                                                                \
    bool stack_##NAME##_empty(stack_##NAME##_t *stack) {                        \
        return stack->top == NULL;                                              \
    }

STACK_DEFINITION(AST_item *, ast, ast_item_destr)
STACK_DEFINITION(Token *, token, tokenDtor)
STACK_DEFINITION(char *, string, free)
STACK_DEFINITION(code_block *, code_block, free)
STACK_DEFINITION(PrecedItem *, precedence, free)

