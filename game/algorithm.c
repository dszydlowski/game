//
//  algorithm.c
//  game
//
//  Created by Daniel Szydłowski on 05/01/2022.
//

#include <stdio.h>
#include <stdlib.h>
typedef enum{
  LEFT_UP,
  UP,
  RIGHT_UP,
  RIGHT,
} E_Direction;

typedef enum{
  empty = 0,
  X,
  O,
  Xblink,
  Oblink
}Pawns;


static Pawns board[7][6] =
{
  {X, X, X, O, empty, empty},
  {X, O, X, X, empty, empty},
  {X, O, empty, empty, empty, empty},
  {X, O, X, empty, empty, empty},
  {O, X, O, empty, empty, empty},
  {O, empty, empty, empty, empty, empty},
  {empty, empty, empty, empty, empty, empty},
};
/*
 struktura linia:
 - ile pol w lini
 - kierunek
 - pole lewe ile brakuje do obstawienia pola: 0 - gotowe do obstawienia, -1 - zablokowane
 - pole prawe/gora ile brakuje do obstawienia
 - pole lewe kolumna (-1 = zablokowane)
 - pole prawe kolumna (-1 = zablokowane)
 */

typedef struct{
  int row;
  int col;
  int pawnsNumber;
  E_Direction direction;
  int leftSide;
  int rightSide;
  int leftCol;
  int rightCol;
  /* fields below are empty */
  int leftEmptyFieldsBelow;
  int rightEmptyFieldsBelow;
}T_Line;
/* ==================================== LIST ==========================================*/
struct List
{
  int index;
  struct List * Next;
  T_Line* Line;
};
typedef struct List T_List;

static T_List* Lines;

T_List* ListInit(void)
{
  T_List* new = (T_List*) malloc(sizeof(T_List));
  new->Next = NULL;
  new->Line = NULL;
  new->index = 0;
  return new;
}

void ListAdd (T_List* ptrList, T_Line* newItem)
{
  T_List* current = ptrList;
  if (newItem == NULL || ptrList == NULL)
  {
    printf ("ListAdd: Problem with empty pointer\n");
    exit(1);
  }
  
  while (current->Line != NULL)
  {
    current = current->Next;
  }
  current->Line = newItem;
  current->Next = (T_List*) malloc (sizeof(T_List));
  current->Next->Next = NULL;
  current->Next->Line = NULL;
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
  free(current->Line);
  if (current->Next != NULL)
  {
    current->Line = current->Next->Line;
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
    current->Line = NULL;
  }
}

T_List* ListLine (T_List* ptrList, int index)
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
  return current;
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
    free(current->Line);
    free(current);
    current = next;
  }
}

/* ==================================== LIST ==========================================*/

void PrintLine (T_Line * line)
{
  printf ("col:%d, row:%d, pawns:%d direction: ", line->col, line->row, line->pawnsNumber);
  switch (line->direction)
  {
    case LEFT_UP:
      printf ("LEFT_UP ");
      break;
    case UP:
      printf ("UP ");
      break;
    case RIGHT_UP:
      printf("RIGHT_UP ");
      break;
    case RIGHT:
      printf("RIGHT ");
      break;
  }
  printf ("beforeBegin %d, beginEmptyFieldsBelow %d, endColumn %d, endEmptyFieldsBelow %d\n",
          line->leftCol, line->leftEmptyFieldsBelow, line->rightCol, line->rightEmptyFieldsBelow);
  
}


/* Function draws whole board according to pawns values on each field. */
void DrawBoard(void)
{
  int i = 0, j = 0;
  /* console cleaning */
  printf("\e[1;1H\e[2J");
  for (j = 5; j >= 0; j--)
  {
    for (i = 0; i < 7; i++)
    {
      switch (board[i][j])
      {
        case empty:
          printf ("[   ]");
          break;
        case X:
          /* Setting font color to green, printing X and setting font color to default */
          printf ("[ \e[32mX\x1b[0m ]");
          break;
        case O:
          /* Setting font color to red, printing O and setting font color to default */
          printf ("[ \x1b[31mO\x1b[0m ]");
          break;
        case Xblink:
          /* Setting font color to blinking green, printing X and set font color to default */
          printf ("[ \e[5m\e[32mX\x1b[0m\e[25m ]");
          board[i][j] = 1;
          break;
        case Oblink:
          /* Font color to blinking red, printing O and set font color to default */
          printf ("[ \e[5m\e[31mO\e[0m\e[25m ]");
          board[i][j] = 2;
          break;
        default:
          printf ("DrawBoard: Incorrect pawn value in board of Pawns\n");
          exit(0);
      }
    }
    printf ("\n");
  }
  printf ("-----------------------------------\n");
  /* Printing column numbers */
  for (i = 0; i < 7; i++)
    printf ("[ %d ]", i);
  printf ("\n");
}


/*  Function returns number of empty fields below field given as an argument.
    If column or row is incorrect then function return -1 */
int SearchFieldsBelow(int column, int row)
{
  int rowIndex;
  int fieldsBelow = 0;
  
  for (rowIndex = row-1; rowIndex >= 0; rowIndex--)
  {
    if (board[column][rowIndex] == empty)
    {
      fieldsBelow++;
    }
    else
    {
      break;
    }
  }
  return fieldsBelow;
}

/*  Function checks filed at arguments colum, row and compares it to player argument
    returns:
    -1 if field has been taken by enemy or does not exists
     0 if field is empty
     1 if fiels belongs to player
 */
int FieldState(int col, int row, Pawns player)
{
  if (col < 0 || col >=7 || row >= 6 || row < 0 )
  {
    /* board border reached - line locked on this side */
    return -1;
  }
  else if (board [col][row] != player && board [col][row] != empty)
  {
    /* field on board has been taken by enemy - line locked on this side */
    return -1;
  }
  else if (board[col][row] == player)
  {
    /* next pawn has been found */
    return 1;
  }
  else if (board [col][row] == empty)
  {
    return 0;
  }
  return 0;
}

/* This function checks filed before the line if it is already taken by enemy, or doesn't exists.
    NewLine instance is completed accordingly*/
void CheckPawnBefore(T_Line* newLine, int col, int row, Pawns player)
{
  /*  TODO: line below can be removed if fldState will be get as an argument */
  int fldState = FieldState(col, row, player);
  
  if (-1 == fldState)
  {
    newLine->rightEmptyFieldsBelow = -1;
    newLine->rightCol = -1;
    newLine->rightSide = -1;
  }
  else /* if 0 == fldState */
  {
    /* File on right end of the line is empty */
    newLine->rightCol = col;
    newLine->rightEmptyFieldsBelow = SearchFieldsBelow(col, row);
  }
  return;
}

void SearchForNextPawnsInLine(T_Line* newLine, int colAdd, int rowAdd)
{
  int fldState;
  int c = colAdd;
  int r = rowAdd;
  for (; c < 4; c+=colAdd, r+=rowAdd)
  {
    fldState = FieldState(newLine->col+c, newLine->row+r, board[newLine->col][newLine->row]);
    if (fldState == -1)
    {
      /* linia zablokowana */
      newLine->leftCol = -1;
      newLine->leftSide = -1;
      newLine->leftEmptyFieldsBelow = -1;
      break;
    }
    else if (fldState == 1)
    {
      newLine->pawnsNumber++;
    }
    else /* if fldState == 0 */
    {
      newLine->leftCol = newLine->col+c;
      newLine->leftEmptyFieldsBelow = SearchFieldsBelow(newLine->col+c, newLine->row+r);
      break;
    }
  }
  return;
}

T_Line* LineInit(int col, int row, E_Direction dir )
{
  T_Line* newLine = (T_Line*) malloc(sizeof (T_Line));
  newLine->col = col;
  newLine->row = row;
  newLine->direction = dir;
  newLine->pawnsNumber = 1;
  newLine->rightEmptyFieldsBelow = 0;
  newLine->leftEmptyFieldsBelow = 0;
  return newLine;
}


void SearchLines (T_List* Lines, Pawns player)
{
  int row, col;
  int fldState = 0;
  T_Line * newLine = NULL;
  
  for (row = 0; row < 6; row ++)
  {
    for (col = 0; col < 7; col++)
    {
      if (board[col][row] == player)
      {
        /* LEFT - UP check for next pawn in line */
        if (col -1 >= 0 && row+1 < 6 && board[col-1][row+1] == player)
        {
          /* next pawn in line found, now check if current pawn is first in line */
          fldState = FieldState(col+1, row-1, player);
          if (fldState != 1)
          {
            /* new line found, create object for it */
            newLine = LineInit(col, row, LEFT_UP);
            CheckPawnBefore(newLine, col+1, row-1, player);
            SearchForNextPawnsInLine(newLine, -1, +1);
            /* add new line to list */
            ListAdd(Lines, newLine);
            PrintLine(newLine);
          }
          else
          {
            /* This pawn is in the middle of another LEFT UP line - new line not created */
          }
        }
        
        /* UP check for next pawn in line */
        if (row+1 < 6 && board [col][row+1] == player)
        {
          fldState = FieldState(col, row-1, player);
          if (fldState != 1)
          {
            newLine = LineInit(col, row, UP);
            CheckPawnBefore(newLine, col, row-1, player);
            SearchForNextPawnsInLine(newLine, 0, +1);
            /* add new line to list */
            ListAdd(Lines, newLine);
            PrintLine(newLine);
          }
          else
          {
            /* This pawn is in the middle of another UP line - new line not created */
          }
        }
        
        /* RIGHT UP */
        if (col+1 < 7 && row+1 < 6 && board[col+1][row+1] == player)
        {
          fldState=FieldState(col-1, row-1, player);
          if (fldState != 1)
          {
            newLine = LineInit(col, row, RIGHT_UP);
            CheckPawnBefore(newLine, col-1, row-1, player);
            SearchForNextPawnsInLine(newLine, +1, +1);
            /* add new line to list */
            ListAdd(Lines, newLine);
            PrintLine(newLine);
          }
          else
          {
            /* this field is a part of another RIGHT UP line - no new line craeted */
          }
        }
        
        /* RIGHT */
        if (col+1 < 7 && board[col+1][row] == player)
        {
          fldState = FieldState(col-1, row, player);
          if (fldState != 1)
          {
            newLine = LineInit(col, row, RIGHT);
            CheckPawnBefore(newLine, col-1, row, player);
            SearchForNextPawnsInLine(newLine, +1, 0);
            /* add newLine to list */
            ListAdd(Lines, newLine);
            PrintLine(newLine);
          }
        }
        else
        {
          /* this field is a part of another RIGHT line  */
        }
        
        
      }
    }
  }
}


char algorithm(void)
{
  T_List * current;
  T_List* enemyLines = ListInit();
  T_List* myLines = ListInit();
  /* szukanie lini przeciwnika */
  SearchLines(enemyLines, X);
  /* szukanie wlasnych linii */
  SearchLines(myLines, O);
  
  current = enemyLines;
  printf ("Enemy Lines:\n");
  while (current->Next != NULL)
  {
    PrintLine(current->Line);
  }
  
  printf ("My Lines:\n");
  current = myLines;
  while (current->Next != NULL)
  {
    PrintLine(current->Line);
  }
  
  /* Szukazanie zakazanych kolumn - czyli takich które po wrzuceniu do nich pionka, pozwolą przeciwnikowi rozbudować linię*/
  /* Analiza lini
      - szukanie wlasnej lini z trzema pionkami i czwartym gotowym do obstawienia - priorytet 0
      - linia przeciwnika z trzema polami i oba czwarte pola są gotowe do obstawienia -
        sytuacja przegrana poleganie na bledzie przeciwnika - priorytet 1
      - linia przeciwnika z trzema pionkami i jednym czwartym polem gotowym do obstawienia - priorytet 2
      - szukanie linii przeciwnika z dwoma pionkami i polami 3 gotowymi do obstawienia - priorytet 3
      - szukanie wlasnej linii z dwoma pionkami i polami 3 gotowymi do obstawienia - priorytet 4
      - szukanie wlasnej lini z trzema pionkami z polami 4 nie gotowymi do obstawienia - priorytet 5
      - szukanie wlasnej lini z dwoma pionkami z polami 3 niegotowymi do obstawienia - p 6
      - przy braku linii szukanie wspolnego miejsca miedzy pojedynczymi pionkami niekoniecznie przylegajacym do już
        postawionych pionków ale w miejscach gdzie potencjalne linie mogą się krzyzowac
   */

  ListFree(enemyLines);
  ListFree(myLines);
  return 'q';
}

int main ()
{
  DrawBoard();
  algorithm();
}
