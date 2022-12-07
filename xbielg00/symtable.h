 /**
 * IFJ22 compiler
 * 
 * @file symtable.h
 * 
 * @brief data structures and functions representing symtable's interface 
 *
 * @author Gabriel Biel(xbielg00)
 */

#ifndef IFJ_SYM_TABLE_H
#define IFJ_SYM_TABLE_H 1

#include <stdbool.h>

#define MAX_HT_SIZE 4999 // highest prime number < 5000

extern int HT_SIZE;


 typedef enum
{
    void_t,
    int_t,
    string_t,
    float_t,
    null_int_t,
    null_string_t,
    null_float_t,
    error
} var_type_t;

typedef struct param_info_t
{
    char *varId;
    var_type_t type;
    struct param_info_t *next;
} param_info_t;


typedef struct
{
    unsigned paramCount;
    param_info_t *params;
    var_type_t returnType;
} fnc_info_t;


/// @brief general item in hash table
typedef struct ht_item {
  bool isfnc;                   // item is function -> fncInfo is relevant
  char *identifier;             // name of the item
  fnc_info_t fnc_data;          // relevant data to item
  unsigned referenceCounter;    // how many times was item referenced
  struct ht_item *next;
} ht_item_t;

typedef struct ht_table_t {
    unsigned size;
    ht_item_t *items[MAX_HT_SIZE];
} ht_table_t;

/// @brief Returns hash for entered key value.
int ht_hash(char *key);

/// @brief Appends new parameter to an existig item.
/// @param appendTo Item to append the parameter to.
/// @param name Id of parameter.
/// @param type Data type of parameter.
void ht_paramAppend(ht_item_t *appendTo, char *name, var_type_t type);

/// @brief Creates pointer to item.
/// @param identifier Id of the item.
/// @param type Return type of function / data type of variable.
/// @param isFunction Switch between function / variable.
/// @returns Pointer to newly created item.
ht_item_t *ht_itemCtor(char* identifier, var_type_t type, bool isFunction);

/// @brief Creates hash table with MAX_HT_SIZE size.
/// @return Pointer to newly created hash table.
ht_table_t *ht_init();

/// @brief Searches symtable for item with coresponding key.
/// @return Pointer to item.
/// @return NULL when item is not in the symtable.
ht_item_t *ht_search(ht_table_t *table, char *key);

/// @brief Inserts or updates item in symtable.
/// @param table Symtable to insert item to.
/// @param identifier Id of the item.
/// @param type Return type of function / data type of variable.
/// @param isFunction Switch between function / variable.
/// @return Pointer to created/updated item in symtable.
/// @return NULL when it tries to redeclare function.
ht_item_t * ht_insert(ht_table_t *table, char* identifier, var_type_t type, bool isFunction) ;

/// @brief Deletes item and frees all its data.
void ht_itemDtor(ht_item_t *item);

/// @brief Deletes item with coresponding key.
void ht_delete(ht_table_t *table, char *key);

/// @brief Deletes whole hash table.
void ht_deleteAll(ht_table_t *table);

#endif // IFJ_SYM_TABLE_H
