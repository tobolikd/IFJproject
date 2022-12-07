/** 
 * IFJ22 compiler
 *
 * @file symtable.c
 * 
 * @brief interface for symtable 
 *
 * @author Gabriel Biel(xbielg00)
 */

#include "symtable.h"
#include "error_codes.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;


int ht_hash(char *key) 
{
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) { // sum of every character x characters index into result
    result += key[i] * (i+1);
  }
  return (result % HT_SIZE);
}


void ht_paramAppend(ht_item_t *appendTo, char *name, var_type_t type)
{
  param_info_t *new = malloc(sizeof(param_info_t)); //create new
  CHECK_MALLOC(new);
  
  new->varId = malloc((strlen(name)+1) * sizeof(char));
  CHECK_MALLOC(new->varId);
  strcpy(new->varId,name); //copy name to varId
  new->type = type;
  new->next = NULL;
  
  if (appendTo->fnc_data.paramCount == 0) //first parameter
  {
    appendTo->fnc_data.params = new;
    appendTo->fnc_data.paramCount = 1;
  }
  else //append param
  {
    param_info_t *cur = appendTo->fnc_data.params;
    appendTo->fnc_data.paramCount++;
    while(cur->next != NULL)          // append as last.
      cur = cur->next; 
    cur->next = new;
  }
  return;
}

ht_item_t *ht_itemCtor(char* identifier, var_type_t type, bool isFunction)
{
  ht_item_t *new = malloc(sizeof(ht_item_t));
  CHECK_MALLOC_PTR(new);
  
  new->identifier = malloc((strlen(identifier)+1) * sizeof(char));
  CHECK_MALLOC_PTR(new->identifier);

  strcpy(new->identifier,identifier); 

  new->isfnc = isFunction;
  new->next = NULL;
  //for variables these data are irelevant
  new->fnc_data.returnType = type;
  new->fnc_data.paramCount = 0;
  new->fnc_data.params = NULL;
  return new;
}

/* HT_TABLE FUNCTIONS*/

ht_table_t *ht_init() 
{
  ht_table_t *table = malloc(sizeof(ht_table_t));
  CHECK_MALLOC_PTR(table);
  table->size = 0;
  for (int i = 0; i < HT_SIZE; i++)
    table->items[i] = NULL;
  return table;
}

ht_item_t *ht_search(ht_table_t *table, char *key) 
{
  int hash = ht_hash(key);
  ht_item_t *item = table->items[hash]; 
  while (item != NULL)
  {
    if (!strcmp(item->identifier,key))
      return item;
    item = item->next;
  }
  return NULL;
}

ht_item_t * ht_insert(ht_table_t *table, char* identifier, var_type_t type, bool isFunction) 
{
  ht_item_t *item = ht_search(table,identifier); // look if exists
  if (item != NULL) // if item already exists in the symtable
  {
    if (isFunction) // double declaration - error
    {
      errorCode = SEMANTIC_FUNCTION_DEFINITION_ERR;
      return NULL; 
    }
    item->referenceCounter++; // increase reference
  }
  else //item does not exists in the symtable
  {
    int hash = ht_hash(identifier); 
    item = ht_itemCtor(identifier,type,isFunction);  //create new item
    
    item->next=table->items[hash]; // link with aliases
    table->items[hash] = item; // insert first
    table->size++;
  }
  return item;
}

void ht_itemDtor(ht_item_t *item)
{
  if (item->isfnc)
  {
    param_info_t *cur = item->fnc_data.params;
    param_info_t *tmp = cur;
    while (cur != NULL) // destroy entire list of parameters
    {
      tmp = cur->next;
      free(cur->varId);
      free(cur);
      cur = tmp;
    }
  }
  free(item->identifier);
  free(item);
}

void ht_delete(ht_table_t *table, char *key) {
  int hash = ht_hash(key);
  ht_item_t *item = table->items[hash];
  ht_item_t *prev = table->items[hash];
  
  while (item != NULL) // item exists
  {
    if (item->identifier == key)// item found
    {
      if (prev == item) // meaning the very first - it must be attached to the table
      {
        table->items[hash] = item->next; //reattach the rest of the list
        ht_itemDtor(item);
        return;
      }
      else  // every other
      {
        prev->next = item->next; //reattach the rest of the list
        ht_itemDtor(item);
        return;
      }
    }
    prev = item;
    item = item->next;
  }
}

void ht_deleteAll(ht_table_t *table) {
  if(table == NULL)
    return;
  ht_item_t *cur = table->items[0];
  ht_item_t *tmp;
  for (int i = 0; i < HT_SIZE; i++)
  {
    cur = table->items[i];
    while (cur != NULL)
    {
      tmp = cur; // attach to destroy later
      cur = cur->next;
      ht_itemDtor(tmp);
    }
    table->items[i] = NULL; //to init state
  }
  free(table);
}
