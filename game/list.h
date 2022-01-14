//
//  list.h
//  game
//
//  Created by Daniel Szydłowski on 14/01/2022.
//

#ifndef list_h
#define list_h

#include <stdio.h>
struct List
{
  int index;
  struct List * Next;
  //T_Line* Line;
  void* Data;
};
typedef struct List T_List;

T_List* ListInit(void);

void ListAdd (T_List* ptrList, void* newItem);

void ListRemove (T_List* ptrList, int index);

void* ListLine (T_List* ptrList, int index);

void ListFree(T_List* ptrList);
#endif /* list_h */
