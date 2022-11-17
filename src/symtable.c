#include <stdlib.h>
#include <string.h>
#include "symtable.h"

int HT_SIZE = MAX_HT_SIZE;

/* SUPPORT FUNCTIONS */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) { // sum of every character x characters index into result
    result += key[i] * (i+1);
  }
  return (result % HT_SIZE); 
}

value_t ht_value_ctor(var_type_t dataType, char* data )
{
  value_t new;

  //attach the correct value based on data type
  if (dataType == int_t || dataType == null_int_t)
  {
    new.intVal = malloc(sizeof(int));
    *new.intVal = atoi(data);
  }
  else if (dataType == float_t || dataType == null_float_t)
  {
    new.intVal = malloc(sizeof(float));
    *new.floatVal = atof(data);
  }
  else if (dataType == string_t || dataType == null_string_t)
  {
    new.intVal = malloc((strlen(data)+1)*sizeof(char));
    new.stringVal = data;
  }
  return new;
}

void ht_param_append(ht_item_t *appendTo, char *name, var_type_t type)
{
  param_info_t *new = malloc(sizeof(param_info_t));
  new->type = type;
  new->varId = name;

  if (appendTo->data.fnc_data.paramCount == 0)
  {
    appendTo->data.fnc_data.params = new;
    appendTo->data.fnc_data.paramCount = 1;
  }
  else
  {
    param_info_t *cur = appendTo->data.fnc_data.params;
    appendTo->data.fnc_data.paramCount++;
    while(cur->next != NULL) // append as last.
      cur = appendTo->data.fnc_data.params->next; 
    cur->next = new;
  }
  return;
}

/// @brief 
/// @param identifier name of the funciton
/// @param type 
/// @param tokenData NULL when function 
/// @param  
/// @return 
ht_item_t *ht_item_ctor(char* identifier, var_type_t type, char *tokenData, bool isFunction)
{
  ht_item_t *new = malloc(sizeof(ht_item_t));
  new->identifier = identifier;
  new->isfnc = isFunction;
  new->next = NULL;
  if (isFunction)
  {
    new->data.fnc_data.returnType = type;
    new->data.fnc_data.paramCount = 0;
    new->data.fnc_data.params = NULL;
  }
  else
  {
    new->data.var_data.value = ht_value_ctor(type,tokenData); 
    new->data.var_data.type = type;
  }
  return new;
}

/* HT_TABLE FUNCTIONS*/

void ht_init(ht_table_t table) {
  for (int i = 0; i < HT_SIZE; i++)
    table[i] = NULL;
}

ht_item_t *ht_search(ht_table_t table, char *key) {
  int hash = get_hash(key);
  ht_item_t *item = table[hash]; 
  while (item != NULL)
  {
    if (item->identifier == key)
      return item;
    item = item->next;
  }
  return item;//either NULL or poinetr
}

void ht_insert(ht_table_t table, ht_item_t *item) 
{
  ht_item_t *searched = ht_search(table,item->identifier); // look if exists
  int hash = get_hash(item->identifier);

  if (searched != NULL) //item exists, replace.
    ht_delete(table,item->identifier);

  item->next=table[hash];
  table[hash] = item; //insert first
}

float *ht_get_float(ht_table_t table, char *key) 
{
  ht_item_t *item = ht_search(table, key);
  
  if (item != NULL)
  {
    if (item->data.var_data.value.floatVal == NULL )
    {
      if (item->data.var_data.type != null_float_t)
      {
        /* ERROR */
      }
    }
    return item->data.var_data.value.floatVal;
  }
  return NULL;
}

int *ht_get_int(ht_table_t table, char *key) {
  ht_item_t *item = ht_search(table, key);
  if (item != NULL)
  {
    return item->data.var_data.value.intVal;
  }
  return NULL;
}

char *ht_get_string(ht_table_t table, char *key) {
  ht_item_t *item = ht_search(table, key);
  if (item != NULL)
  {
    return item->data.var_data.value.stringVal;
  }
  return NULL;
}

void ht_item_dtor(ht_item_t *item)
{
  if (item->isfnc)
  {
    param_info_t *cur = item->data.fnc_data.params;
    param_info_t *tmp = cur;
    while (cur != NULL)
    {
      free(cur);
      cur = tmp->next;
      tmp = cur;
    }
  }
  else
  {
    if (item->data.var_data.type == int_t || item->data.var_data.type == null_int_t)
      free(item->data.var_data.value.intVal);
    else if (item->data.var_data.type == float_t || item->data.var_data.type == null_float_t)
      free(item->data.var_data.value.floatVal);
    else if (item->data.var_data.type == string_t || item->data.var_data.type == null_string_t)
      free(item->data.var_data.value.stringVal);
  }
  free(item);
}

void ht_delete(ht_table_t table, char *key) {
  int hash = get_hash(key);
  ht_item_t *item = table[hash];
  ht_item_t *prev = table[hash];
  
  while (item != NULL) // it might exist 
  {
    if (item->identifier == key)// del me
    {
      if (prev == item) // meaning the very first
      {
        table[hash] = item->next;
        ht_item_dtor(item);
      }
      else  
      {
        if (item->next != NULL)
          prev->next = item->next;
        else
          prev->next = NULL;

        ht_item_dtor(item);
      }
      return;
    }
    prev = item;
    item = item->next;
  }
}

void ht_delete_all(ht_table_t table) {
  for (int i = 0; i < HT_SIZE; i++)
  {
    ht_item_t *destroyLater[255];
    *destroyLater=NULL;
    int count = 0;
    ht_item_t *cur = table[i];

    while (cur != NULL)
    {
      destroyLater[count++]=cur; //attach to destroy later
      cur =cur->next;
    }
    if (*destroyLater!=NULL)
      for (int j = 0; j < count; j++)
      {
        ht_item_dtor(destroyLater[j]);
        destroyLater[j] = NULL;
      }
    table[i] = NULL; //to init state
  }
}

/* HT_LIST FUNCTIONS*/

void ht_list_init(ht_list_t *list)
{
  list = NULL;
  list->len = 0;
}

/// @brief Push new ht_table to the list.
void ht_list_push(ht_table_t table , ht_list_t *list)
{
  if (list->len == 0) // empty
  {
    list->len = 1;
    list->table[0] = malloc(1 *sizeof(ht_table_t));
    list->table[0] = *table;
  }
  else
  {
    list = realloc(list,++list->len * sizeof(ht_table_t));
    list->table[list->len - 1] = *table;
  }
}

/// @brief Delete the latest ht_table.
void ht_list_pop(ht_list_t *list)
{
  if (list->len != 0)
  {
    ht_delete_all(&list->table[list->len -1]); //delete the latest
    list->table[list->len -1] = NULL;
    list->len--;
  }
}

/// @brief Returns the lastest declared item with wanted key.
/// @return Item when found. Null if item is not in the list. 
ht_item_t *ht_list_search(ht_list_t *list, char * key)
{
  ht_item_t *item = NULL;
  for (int i = list->len - 1; i >=0; i++)
  {
    item = ht_search(&list->table[i],key); // search in every table from the latest to newest
    if (item != NULL)
      return item;
  }
  return NULL;  
}

/// @brief Return value of the latest declared variable.
/// @return NULL if there is not wanted item.
/// @return Pointer to float value.
float *ht_list_get_float(ht_list_t* list, char *key)
{
  float *value = NULL;
  for (int i = list->len -1; i >= 0; i++)
  {
    value = ht_get_float(&list->table[i],key);
    if (value != NULL)
      return value;
  }
  return NULL;
}

/// @brief Return value of the latest declared variable.
/// @return NULL if there is not wanted item.
/// @return Pointer to int value.
int *ht_list_get_int(ht_list_t* list, char *key)
{
  int *value = NULL;
  for (int i = list->len -1; i >= 0; i++)
  {
    value = ht_get_int(&list->table[i],key);
    if (value != NULL)
      return value;
  }
  return NULL;
}

/// @brief Return value of the latest declared variable. 
/// @return NULL if there is not wanted item. 
/// @return Pointer to string. 
char *ht_list_get_string(ht_list_t* list, char *key)
{
  char *value = NULL;
  for (int i = list->len -1; i >= 0; i++)
  {
    value = ht_get_string(&list->table[i],key);
    if (value != NULL)
      return value;
  }
  return NULL;
}

