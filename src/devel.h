#ifndef IFJ_DEVEL_H
#define IFJ_DEVEL_H 1

#include <stdio.h>

#include "error-codes.h"
#include "ast.h"

/* printAstItem
 *
 * prints AST item with type and data information to stderr
 * active if DEBUG == 1
 */
void printAstItem(AST_item *item);


#endif // IFJ_DEVEL_H
