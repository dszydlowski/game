//
//  board.h
//  game
//
//  Created by Daniel Szydłowski on 14/01/2022.
//

#ifndef board_h
#define board_h

#include <stdio.h>

typedef enum{
  empty = 0,
  X,
  O,
  Xblink,
  Oblink
}Pawns;

typedef enum{
  playerError = -3,
  columnFull = -2,
  incorrectColumnNumber = -1,
  PawnPutCorrectly = 0,
} BoardErrors;

extern Pawns board[7][6];

/* Function draws whole board according to pawns values on each field. */
void DrawBoard(void);

/* Function puts user's pown on selected column. Function responsible for
 simulation of gravity - that means pawn is always put on possible lowest level.
 RETURNS:
  0 (PawnPutCorrectly) - pawn putted on board correctly
 -1 (incorrectColumnNumber) - returned if argument 'column' is higher than 6 or lower than 0,
 -2 (columnFull) - returned if column is full and it is not possible to put any more pawns
 -3 (playerError) - returned if player id passed as an argument is lower than 1 or
    higher than 2 */
BoardErrors PutPownOnBoard (int column, Pawns player);

/* Use this function to inform player that choosen column is full.
 All pawns in column passed in argument will be set to blinking.
 Function returns error if selected column is not full.
 */
void SetColumnAsBlinking(int column);

#endif /* board_h */
