#ifndef IFJ_SYM_TABLE_H
#define IFJ_SYM_TABLE_H

#include <stdbool.h>
#include "lex-analyzer.h"
#include "syn-analyzer.h"

#define MAX_HT_SIZE 251 // highest prime number < 256

extern int HT_SIZE;


 typedef enum
{
    void_t,
    int_t,
    string_t,
    float_t,
    null_int_t,
    null_string_t,
    null_float_t
} var_type_t;

typedef struct param_info_t
{
    char *varId;
    var_type_t type;
    struct param_info_t *next;
} param_info_t;

typedef struct
{
    var_type_t type;
    char *varId;
} var_info_t;

typedef struct
{
    unsigned paramCount;
    param_info_t *params;
    var_type_t returnType;
} fnc_info_t;

typedef union 
{
  var_info_t var_data;
  fnc_info_t fnc_data;
} symbol_data;

/// @brief general item in hash table - could be variable or fnc 
typedef struct ht_item {
  bool isfnc;                 
  char *identifier;
  symbol_data data;
  unsigned referenceCounter;
  struct ht_item *next;     // next alias
} ht_item_t;

typedef struct ht_table_t {
    unsigned size;        
    ht_item_t *items[MAX_HT_SIZE];         
} ht_table_t;

/// @brief hash table list to keep track of frameworks 
/// @brief Returns hash for key. 
int get_hash(char *key);

/// @brief Appends new parameter to an existig item.
/// @param appendTo Item to append the parameter to.
/// @param name Id of parameter.
/// @param type Data type of parameter.
void ht_param_append(ht_item_t *appendTo, char *name, var_type_t type);

/// @brief Creates pointer to item.
/// @param identifier Id of the item.
/// @param type Return type of function / data type of variable.
/// @param tokenData Data of token. Stored in string.
/// @param isFunction Switch between function / variable.
ht_item_t *ht_item_ctor(char* identifier, var_type_t type, bool isFunction);

/// @brief Creates hash table with MAX_HT_SIZE size.
/// @return Pointer to hash table.
ht_table_t *ht_init() ;

/// @brief Searches table for wanted item.
/// @return Pointer to item or NULL.
ht_item_t *ht_search(ht_table_t *table, char *key);

/// @brief Insert item to table as first item in alias list.
ht_item_t * ht_insert(ht_table_t *table, char* identifier, var_type_t type, bool isFunction) ;

/// @brief Deletes item. Used in ht_delete
void ht_item_dtor(ht_item_t *item);

/// @brief Deletes item with coresponding key. 
void ht_delete(ht_table_t *table, char *key);

/// @brief Deletes whole hash table.
void ht_delete_all(ht_table_t *table);

#endif // IFJ_SYM_TABLE_H
