#ifndef IFJ_DEVEL_H
#define IFJ_DEVEL_H 1

#include <stdio.h>

#include "ast.h"
#include "stack.h"

/* printAstItem
 *
 * prints AST item with type and data information to stderr
 * active if DEBUG == 1
 */
void printAstItem(AST_item *item);

/* printAstFnc
 *
 * prints function call context
 */
void printAstFnc(AST_function_call_data *data);

void printAstStack(stack_ast_t *stack);

void printCodeBlockStack(stack_code_block_t *stack);

#endif // IFJ_DEVEL_H
