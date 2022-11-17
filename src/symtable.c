#include "symtable.h"
#include "error-codes.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/* SUPPORT FUNCTIONS */
int get_hash(char *key) 
{
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) { // sum of every character x characters index into result
    result += key[i] * (i+1);
  }
  return (result % HT_SIZE);
}


void ht_param_append(ht_item_t *appendTo, char *name, var_type_t type)
{
  param_info_t *new = malloc(sizeof(param_info_t)); //create new
  CHECK_MALLOC(new);
  
  new->varId = malloc((strlen(name)+1) * sizeof(char));
  CHECK_MALLOC(new->varId);
  strcpy(new->varId,name); //copy name to varId
  new->type = type;
  
  if (appendTo->data.fnc_data.paramCount == 0) //first parameter
  {
    appendTo->data.fnc_data.params = new;
    appendTo->data.fnc_data.paramCount = 1;
  }
  else //append param
  {
    param_info_t *cur = appendTo->data.fnc_data.params;
    appendTo->data.fnc_data.paramCount++;
    while(cur->next != NULL)          // append as last.
      cur = appendTo->data.fnc_data.params->next;
    cur->next = new;
  }
  return;
}

ht_item_t *ht_item_ctor(char* identifier, var_type_t type, bool isFunction)
{
  ht_item_t *new = malloc(sizeof(ht_item_t));
  CHECK_MALLOC(new);
  
  new->identifier = malloc((strlen(identifier)+1) * sizeof(char));
  CHECK_MALLOC(new->identifier);

  strcpy(new->identifier,identifier); 

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
    new->data.var_data.type = type;
  }
  return new;
}

/* HT_TABLE FUNCTIONS*/

ht_table_t *ht_init() 
{
  ht_table_t *table = malloc(sizeof(ht_table_t));
  CHECK_MALLOC(table);
  table->size = 0;
  for (int i = 0; i < HT_SIZE; i++)
    table->items[i] = NULL;
  return table;
}

ht_item_t *ht_search(ht_table_t *table, char *key) 
{
  int hash = get_hash(key);
  ht_item_t *item = table->items[hash]; 
  while (item != NULL)
  {
    if (item->identifier == key)
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
    item->data.var_data.type = type; // update data
  }
  else //item does not exists in the symtable
  {
    unsigned hash = get_hash(item->identifier); 
    item = ht_item_ctor(identifier,type,isFunction);  //create new item
    
    item->next=table->items[hash]; // link with aliases
    table->items[hash] = item; // insert first
  }
  return item;
}

void ht_item_dtor(ht_item_t *item)
{
  if (item->isfnc)
  {
    param_info_t *cur = item->data.fnc_data.params;
    param_info_t *tmp = cur;
    while (cur != NULL) // destroy entire list
    {
      free(cur->varId);
      free(cur);
      cur = tmp->next;
      tmp = cur;
    }
  }
  free(item->identifier);
  free(item);
}

void ht_delete(ht_table_t *table, char *key) {
  int hash = get_hash(key);
  ht_item_t *item = table->items[hash];
  ht_item_t *prev = table->items[hash];

  while (item != NULL) // item exists
  {
    if (item->identifier == key)// item found
    {
      if (prev == item) // meaning the very first - it must be attached to the table
      {
        table->items[hash] = item->next; //reattach the rest of the list
        ht_item_dtor(item);
        return;
      }
      else  // every other
      {
        prev->next = item->next; //reattach the rest of the list
        ht_item_dtor(item);
        return;
      }
    }
    prev = item;
    item = item->next;
  }
}

void ht_delete_all(ht_table_t *table) {
  for (int i = 0; i < HT_SIZE; i++)
  {
    ht_item_t *destroyLater[255]; //support array to temporary store every item
    *destroyLater=NULL;
    int count = 0;
    ht_item_t *cur = table->items[i];

    while (cur != NULL)
    {
      destroyLater[count++]=cur; // attach to destroy later
      cur =cur->next; // go to next alias
    }
    if (*destroyLater!=NULL)
      for (int j = 0; j < count; j++) // free every item temporary stored
      {
        ht_item_dtor(destroyLater[j]);
        destroyLater[j] = NULL;
      }
    table->items[i] = NULL; //to init state
  }
  free(table);
}
