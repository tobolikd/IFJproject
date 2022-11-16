#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/* SUPPORT FUNCTIONS */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) { // sum of every character x characters index into result
    result += key[i] * i;
  }
  return (result % HT_SIZE); 
}

value_t ht_create_value(TokenType dataType, char* value, bool isNullType )
{
  value_t new;
  new.floatVal = NULL;
  new.intVal = NULL;
  new.stringVal = NULL;

  //these data types may contain NULL
  if (isNullType)
    new.nullType = true;
  else
    new.nullType = false;

  //attach the correct value based on data type
  if (dataType == t_int)
    *new.intVal = atoi(value);
  else if (dataType == t_float)
    *new.floatVal = atof(value);
  else if (dataType == t_string)
    new.stringVal = value;

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
    if (item->key == key)
      return item;
    if (item->next != NULL)
      item = item->next;
    else 
      return NULL;
  }
  return item;//either NULL or poinetr
}

void ht_insert(ht_table_t table, char *key, value_t value) {
  ht_item_t *item = ht_search(table,key); // look if exists
  if (item == NULL) // key doesnt exists in the table
  {
    int hash = get_hash(key);
    ht_item_t *new = malloc(sizeof(ht_item_t)); //create new
    new->key=key;
    new->value=value;
    new->next=table[hash];
    table[hash] = new; //insert first
  }
  else //there already exists key
  {
    item->value = value;
  }
}

float *ht_get_float(ht_table_t table, char *key) {
  ht_item_t *item = ht_search(table, key);
  if (item != NULL)
  {
    return item->value.floatVal;
  }
  return NULL;
}

int *ht_get_int(ht_table_t table, char *key) {
  ht_item_t *item = ht_search(table, key);
  if (item != NULL)
  {
    return item->value.intVal;
  }
  return NULL;
}

char *ht_get_string(ht_table_t table, char *key) {
  ht_item_t *item = ht_search(table, key);
  if (item != NULL)
  {
    return item->value.stringVal;
  }
  return NULL;
}

void ht_delete(ht_table_t table, char *key) {
  int hash = get_hash(key);
  ht_item_t *item = table[hash];
  ht_item_t *prev = table[hash];
  
  while (item != NULL) // it might exist 
  {
    if (item->key == key)// del me
    {
      if (prev == item) // meaning the very first
      {
        if (item->next != NULL)
          table[hash] = item->next;
        else
          table[hash] = NULL;
        free(item->value.floatVal);
        free(item->value.intVal);
        free(item->value.stringVal);
        free(item);
        return;
      }
      else  
      {
        if (item->next != NULL)
          prev->next = item->next;
        else
          prev->next = NULL;

        free(item->value.floatVal);
        free(item->value.intVal);
        free(item->value.stringVal);
        free(item);

        return;
      }
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
        free(destroyLater[j]->value.floatVal);
        free(destroyLater[j]->value.stringVal);
        free(destroyLater[j]->value.intVal);
        free(destroyLater[j]);
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

