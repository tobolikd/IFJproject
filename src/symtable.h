#ifndef IFJ_SYM_TABLE_H
#define IFJ_SYM_TABLE_H

#include <stdbool.h>
#include "lex-analyzer.h"
#include "syn-analyzer.h"

#define MAX_HT_SIZE 251 // highest prime number < 256

extern int HT_SIZE;

/// @brief access to all data types
typedef union{
  char *stringVal;    
  float *floatVal;     
  int *intVal;
} value_t ;

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
    value_t value;
} var_info_t;

typedef struct
{
    int paramCount;
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
  struct ht_item *next;     // next alias
} ht_item_t;

/// @brief hash table 
typedef ht_item_t *ht_table_t[MAX_HT_SIZE];

/// @brief hash table list to keep track of frameworks 
typedef struct 
{
  ht_table_t table; //list of hash tables
  int len;
} ht_list_t;

/// @brief Returns hash for key. 
int get_hash(char *key);

/// @brief Creates ht_value_t structure.
/// @param data Data to be converted into its respective value.
value_t ht_value_ctor(var_type_t dataType, char* data );

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
ht_item_t *ht_item_ctor(char* identifier, var_type_t type, char *tokenData, bool isFunction);

/// @brief Initiates table. 
void ht_init(ht_table_t table);

/// @brief Searches table for wanted item.
/// @return Pointer to item or NULL.
ht_item_t *ht_search(ht_table_t table, char *key);

/// @brief Insert item to table as first item in alias list.
void ht_insert(ht_table_t table, ht_item_t *item);

/// @brief Return value .
/// @return NULL if there is not wanted item.
/// @return Pointer to float value.
float *ht_get_float(ht_table_t table, char *key);

/// @brief Return value.
/// @return NULL if there is not wanted item.
/// @return Pointer to int value.
int *ht_get_int(ht_table_t table, char *key);

/// @brief Return value.
/// @return NULL if there is not wanted item.
/// @return Pointer to string.
char *ht_get_string(ht_table_t table, char *key);

/// @brief Deletes item. Used in ht_delete
void ht_item_dtor(ht_item_t *item);

/// @brief Deletes item with coresponding key. 
void ht_delete(ht_table_t table, char *key);

/// @brief Deletes whole hash table.
void ht_delete_all(ht_table_t table);

/* LIST */
void ht_list_init(ht_list_t *table);

/// @brief Push new ht_table to the list.
void ht_list_push(ht_table_t table , ht_list_t *list);

/// @brief Pop the latest ht_table.
void ht_list_pop(ht_list_t *list);

/// @brief Returns the lastest declared item with wanted key.
/// @return Item when found. Null if item is not in the list. 
ht_item_t *ht_list_search(ht_list_t *list, char * key);

/// @brief Return value of the latest declared variable.
/// @return NULL if there is not wanted item.
/// @return Pointer to float value.
float *ht_list_get_float(ht_list_t *list, char *key);

/// @brief Return value of the latest declared variable.
/// @return NULL if there is not wanted item.
/// @return Pointer to int value.
int *ht_list_get_int(ht_list_t *list, char *key);

/// @brief Return value of the latest declared variable. 
/// @return NULL if there is not wanted item. 
/// @return Pointer to string. 
char *ht_list_get_string(ht_list_t *list, char *key);

#endif // IFJ_SYM_TABLE_H