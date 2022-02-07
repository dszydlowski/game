//
//  board.c
//  game
//
//  Created by Daniel Szydłowski on 14/01/2022.
//
#include <stdlib.h>
#include "board.h"

Pawns board[7][6] =
{
  {empty, empty, empty, empty, empty, empty},
  {empty, empty, empty, empty, empty, empty},
  {empty, empty, empty, empty, empty, empty},
  {empty, empty, empty, empty, empty, empty},
  {empty, empty, empty, empty, empty, empty},
  {empty, empty, empty, empty, empty, empty},
  {empty, empty, empty, empty, empty, empty},
};


/* Function draws whole board according to pawns values on each field. */
void DrawBoard(void)
{
  int i = 0, j = 0;
  /* console cleaning */
#ifndef DEBUG
  printf("\e[1;1H\e[2J");
#endif
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
#ifndef DEBUG
          printf ("[ \e[32mX\x1b[0m ]");
#else
          printf ("[ X ]");
#endif
          break;
        case O:
          /* Setting font color to red, printing O and setting font color to default */
#ifndef DEBUG
          printf ("[ \x1b[31mO\x1b[0m ]");
#else
          printf ("[ O ]");
#endif
          break;
        case Xblink:
          /* Setting font color to blinking green, printing X and set font color to default */
#ifndef DEBUG
          printf ("[ \e[5m\e[32mX\x1b[0m\e[25m ]");
#else
          printf ("[ X ]");
#endif
          board[i][j] = 1;
          break;
        case Oblink:
          /* Font color to blinking red, printing O and set font color to default */
#ifndef DEBUG
          printf ("[ \e[5m\e[31mO\e[0m\e[25m ]");
#else
          printf ("[ O ]");
#endif
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

/* Function puts user's pown on selected column. Function responsible for
 simulation of gravity - that means pawn is always put on possible lowest level.
 RETURNS:
  0 (PawnPutCorrectly) - pawn putted on board correctly
 -1 (incorrectColumnNumber) - returned if argument 'column' is higher than 6 or lower than 0,
 -2 (columnFull) - returned if column is full and it is not possible to put any more pawns
 -3 (playerError) - returned if player id passed as an argument is lower than 1 or
    higher than 2 */
BoardErrors PutPownOnBoard (int column, Pawns player)
{
  int i = 0;
  int fieldWasSet = 0;
  if (column > 6 || column < 0)
  {
    /* incorrect column number */
    return incorrectColumnNumber;
  }
  
  if (player < X || player > O)
  {
    /* incorrect player id */
    printf ("PutPownOnBoard: Incorrect player ID\n");
    exit(0);
  }
  
  for (i = 0; i < 6; i++)
  {
    if (board[column][i] == 0)
    {
      /* empty field has been found in column */
      board[column][i] = player;
      fieldWasSet = 1;
      break;
    }
  }
  
  if (fieldWasSet == 0)
  {
    /* Column is full */
    return columnFull;
  }
  return PawnPutCorrectly;
}

/* Use this function to inform player that choosen column is full.
 All pawns in column passed in argument will be set to blinking.
 Function returns error if selected column is not full.
 */
void SetColumnAsBlinking(int column)
{
  int i = 0;
  for (i = 0; i < 6; i++)
  {
    if (board[column][i] == 0)
    {
      printf ("setColumnAsBlinking: Column not full!\n");
      /* todo: return correct error message. */
      return;
    }
    /*  setting column as blinking by adding enum O = 2 */
    board[column][i] += O;
  }
}

/* removed all pawns - game restart  */
void ClearArea (void)
{
  int i = 0, j = 0;
  for (j = 5; j >= 0; j--)
  {
    for (i = 0; i < 7; i++)
    {
      board[i][j] = empty;
    }
  }
}
