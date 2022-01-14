//
//  list.c
//  game
//
//  Created by Daniel Szydłowski on 14/01/2022.
//
#include <stdlib.h>
#include "list.h"

T_List* ListInit(void)
{
  T_List* new = (T_List*) malloc(sizeof(T_List));
  new->Next = NULL;
  new->Data = NULL;
  new->index = 0;
  return new;
}

void ListAdd (T_List* ptrList, void* newItem)
{
  T_List* current = ptrList;
  if (newItem == NULL || ptrList == NULL)
  {
    printf ("ListAdd: Problem with empty pointer\n");
    exit(1);
  }
  
  while (current->Data != NULL)
  {
    current = current->Next;
  }
  current->Data = newItem;
  current->Next = (T_List*) malloc (sizeof(T_List));
  current->Next->Next = NULL;
  current->Next->Data = NULL;
  current->Next->index = current->index +1;
  return;
}

void ListRemove (T_List* ptrList, int index)
{
  T_List* current = ptrList;
  T_List* tmpNext = NULL;
  if (ptrList == NULL)
  {
    printf ("ListRemove: Problem with empty pointer\n");
    exit(1);
  }
  
  while (current->index != index)
  {
    current = current->Next;
  }
  free(current->Data);
  if (current->Next != NULL)
  {
    current->Data = current->Next->Data;
    tmpNext = current->Next->Next;
    free(current->Next);
    current->Next = tmpNext;
    /* updating indexes */
    current = current->Next;
    while (current == NULL)
    {
      current->index--;
      current = current->Next;
    }
    
  }
  else
  {
    current->Data = NULL;
  }
}

void* ListLine (T_List* ptrList, int index)
{
  T_List* current = ptrList;
  if (ptrList == NULL)
  {
    printf ("ListRemove: Problem with empty pointer\n");
    exit(1);
  }
  
  while (current != NULL && current->index != index)
  {
    current = current->Next;
  }
  return current->Data;
}

void ListFree(T_List* ptrList)
{
  T_List* current = ptrList;
  T_List* next = NULL;
  if (ptrList == NULL)
  {
    printf ("ListRemove: Problem with empty pointer\n");
    exit(1);
  }
  
  while(current != NULL)
  {
    next = current->Next;
    free(current->Data);
    free(current);
    current = next;
  }
}
