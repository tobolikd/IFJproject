/**
 * IFJ22 compiler
 * 
 * @file devel.h
 * 
 * @brief developer library for formatted printing of ast and code block stack
 *
 * @author David Tobolik (xtobol06)
 */

#ifndef IFJ_DEVEL_H
#define IFJ_DEVEL_H 1

#include <stdio.h>

#include "ast.h"
#include "stack.h"

/* print_ast_item
 *
 * prints AST item with type and data information to stderr
 * active if DEBUG == 1
 */
void print_ast_item(AST_item *item);

/* print_ast_fnc
 *
 * prints function call context
 */
void print_ast_fnc(AST_function_call_data *data);

/* print_ast_stack
 * 
 * prints formatted ast stack
 */
void print_ast_stack(stack_ast_t *stack);

/* print_code_block_stack
 * 
 * prints formatted code block stack
 */
void print_code_block_stack(stack_code_block_t *stack);

#endif // IFJ_DEVEL_H
