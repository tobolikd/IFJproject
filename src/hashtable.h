#include <stdbool.h>
#include "lex-analyzer.h"

#define MAX_HT_SIZE 251 // highest prime number < 256

extern int HT_SIZE;

/// @brief data type for variable
typedef struct{
  TokenType dataType;    // expect t_int t_float t_string
  char *name;             
} variable_t ;

/// @brief access to all data types
typedef struct{
  char *stringVal;    
  float *floatVal;     
  int *intVal;
  bool nullType;            // can be null - "?" declaration
} value_t ;

/// @brief general item in hash table - could be variable or function 
typedef struct ht_item {
  char *key;                 // function / variable name
  //return type / variable type
  TokenType dataType;       // expect t_int t_float t_string
  
  //for Function
  bool declared;            // if declaration => true
  variable_t *argument;     // list of arguments

  //for Variables
  value_t value;            // value of variable
  
  struct ht_item *next;     // next alias
} ht_item_t;


/// @brief hash table 
typedef ht_item_t *ht_table_t[MAX_HT_SIZE];

/// @brief hash table list to keep track of frameworks 
typedef struct 
{
  ht_table_t table; //list of hash tables
  int len;
}ht_list_t;
 

/// @brief Returns hash for key. 
int get_hash(char *key);

/// @brief Initiates table. 
void ht_init(ht_table_t table);

/// @brief Generates value_t. 
value_t ht_create_value(TokenType dataType, char* value, bool isNullType);

/// @brief Searches table for wanted item.
/// @return Pointer to item or NULL.
ht_item_t *ht_search(ht_table_t table, char *key);

/// @brief Insert item to table as first item in alias list.
void ht_insert(ht_table_t table, char *key, value_t value);

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

/// @brief Deletes key item. 
void ht_delete(ht_table_t table, char *key);

/// @brief Deletes whole hash table.
void ht_delete_all(ht_table_t table);

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

