//
//  algorithm.c
//  game
//
//  Created by Daniel Szydłowski on 05/01/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "list.h"


typedef enum{
  LEFT_UP,
  UP,
  RIGHT_UP,
  RIGHT,
} E_Direction;


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
  /* sides not used - probably to be removed */
  int leftSide;
  int rightSide;
  int leftCol;
  int rightCol;
  /* fields below are empty */
  int leftEmptyFieldsBelow;
  int rightEmptyFieldsBelow;
  /* priority will be set during analysis, -1 = not set */
  int priority;
}T_Line;

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
  printf ("beforeBegin %d, beginEmptyFieldsBelow %d, endColumn %d, endEmptyFieldsBelow %d, priority: %d\n",
          line->leftCol, line->leftEmptyFieldsBelow, line->rightCol, line->rightEmptyFieldsBelow, line->priority);
}

/*  Function returns number of empty fields below field given as an argument.
    If column or row is incorrect then function return -1 */
static int SearchFieldsBelow(int column, int row)
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
static int FieldState(int col, int row, Pawns player)
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
static void CheckPawnBefore(T_Line* newLine, int col, int row, Pawns player)
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

static void SearchForNextPawnsInLine(T_Line* newLine, int colAdd, int rowAdd)
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

static T_Line* LineInit(int col, int row, E_Direction dir )
{
  T_Line* newLine = (T_Line*) malloc(sizeof (T_Line));
  newLine->col = col;
  newLine->row = row;
  newLine->direction = dir;
  newLine->pawnsNumber = 1;
  newLine->rightEmptyFieldsBelow = 0;
  newLine->leftEmptyFieldsBelow = 0;
  newLine->priority = -1;
  return newLine;
}


static void SearchLines (T_List* Lines, Pawns player)
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

static int PriorityCheck(T_Line* line, int isMyList)
{
  /* Szukazanie zakazanych kolumn - czyli takich które po wrzuceniu do nich pionka, pozwolą przeciwnikowi rozbudować linię*/
  //      - przy braku linii szukanie wspolnego miejsca miedzy pojedynczymi pionkami niekoniecznie przylegajacym do już
  //        postawionych pionków ale w miejscach gdzie potencjalne linie mogą się krzyzowac
  if (line == NULL)
  {
    return -1;
  }
//      - szukanie wlasnej lini z trzema pionkami i czwartym gotowym do obstawienia - priorytet 0
  else if (line->pawnsNumber == 3 && isMyList != 0 && (line->leftEmptyFieldsBelow == 0 || line->rightEmptyFieldsBelow == 0))
  {
    line->priority = 0;
  }
//      - linia przeciwnika z trzema polami i oba czwarte pola są gotowe do obstawienia - sytuacja przegrana poleganie na bledzie przeciwnika - priorytet 1
  else if (isMyList == 0 && line->pawnsNumber == 3 && (line->leftEmptyFieldsBelow == 0 && line->rightEmptyFieldsBelow == 0) )
  {
    line->priority = 1;
  }
//      - linia przeciwnika z trzema pionkami i jednym czwartym polem gotowym do obstawienia - priorytet 2
  else if (isMyList == 0 && line->pawnsNumber == 3 && (line->leftEmptyFieldsBelow == 0 ^ line->rightEmptyFieldsBelow == 0))
  {
    line->priority = 2;
  }
//      - szukanie linii przeciwnika z dwoma pionkami i polami 3 gotowymi do obstawienia - priorytet 3
  else if (isMyList == 0 && line->pawnsNumber == 2 && (line->leftEmptyFieldsBelow == 0 || line->rightEmptyFieldsBelow == 0))
  {
    line->priority = 3;
  }
//      - szukanie wlasnej linii z dwoma pionkami i polami 3 gotowymi do obstawienia - priorytet 4
  else if (isMyList != 0 && line->pawnsNumber == 2 && line->leftEmptyFieldsBelow == 0 && line->rightEmptyFieldsBelow == 0)
  {
    line->priority = 4;
  }
//      - szukanie wlasnej lini z trzema pionkami z polami 4 nie gotowymi do obstawienia - priorytet 5
  else if (isMyList != 0 && line->pawnsNumber == 3 && (line->leftEmptyFieldsBelow > 0 || line->rightEmptyFieldsBelow > 0))
  {
    line->priority = 5;
  }
//      - szukanie wlasnej lini z dwoma pionkami z polami 3 niegotowymi do obstawienia - p 6
  else if (isMyList != 0 && line->pawnsNumber == 2 && (line->leftEmptyFieldsBelow > 0 || line->rightEmptyFieldsBelow > 0))
  {
    line->priority = 6;
  }
  // linia zblokowana z obu stron
  else if (line->rightCol == -1 && line->leftCol == -1)
  {
    line->priority = 101;
  }
  else{
    line->priority = 100;
  }
return line->priority;
}

/*  This function search forbidden columns - thah column if player put pawn into it then opponent will
    collect 4 pawns in line. Argument takes list of enemy lines. Returns forbidden columns bitcoded. */
int ForbiddenColumns (T_List* list)
{
  int i;
  int forbidden = 0;
  T_Line* current;
  for (i = 0; 1; i++)
  {
    current = ListLine(list, i);
    if (current == NULL)
      break;
    if (current->pawnsNumber >= 3)
    {
      if (current->leftCol != -1 && current->leftEmptyFieldsBelow == 1)
      {
        forbidden |= (1 << current->leftCol);
      }
      if (current->rightCol != -1 && current->leftEmptyFieldsBelow == 1)
      {
        forbidden |= (1 << current->rightCol);
      }
    }
  }
  return forbidden;
}

char Algorithm(void)
{
  T_Line * current;
  T_List* enemyLines = ListInit();
  T_List* myLines = ListInit();
  int returnColumn = -1;
  int forbiddenColumns = 0;
  int i = 0;
  int currentPriority = -1;
  int lowestPriority = 1000;
  int lowestIndex = -1;
  int lowestIndexMyList = -1;
  /* szukanie lini przeciwnika */
  SearchLines(enemyLines, X);
  /* szukanie wlasnych linii */
  SearchLines(myLines, O);
  
  /* przeniesc do to osobnej funkcji, ktora zwroci numer kolumny */
  printf ("Enemy Lines:\n");
  for (i = 0; 1; i++)
  {
    current = ListLine(enemyLines, i);
    if (current == NULL)
      break;
    PrintLine(current);
    currentPriority = PriorityCheck(current, 0);
    if (currentPriority < lowestPriority)
    {
      lowestPriority = currentPriority;
      lowestIndex = i;
      lowestIndexMyList = 0;
    }
  }
  
  printf ("My Lines:\n");
  for (i = 0; 1; i++)
  {
    current = ListLine(myLines, i);
    if (current == NULL)
      break;
    PrintLine(current);
    currentPriority = PriorityCheck(current, 1);
    if (currentPriority < lowestPriority)
    {
      lowestPriority = currentPriority;
      lowestIndex = i;
      lowestIndexMyList = 1;
    }
  }
  
  forbiddenColumns = ForbiddenColumns(enemyLines);
  if (forbiddenColumns == 127)
  {
    printf ("error: all collumns forbidden, game quit\n");
    return 'q';
  }
  
  if (lowestPriority < 100)
  {
    if (lowestIndexMyList == 0)
      current = ListLine(enemyLines, lowestIndex);
    else
      current = ListLine(myLines, lowestIndex);
    
    /* search for column */
                         
  }
  else
  {
    /* random column and check if it is not forbidden column */
    do{
      returnColumn = rand() % 7;
    }while ((forbiddenColumns >> returnColumn) && 0x00000001);
  }
  ListFree(enemyLines);
  ListFree(myLines);
  return (char) (returnColumn+48);
}
