//
//  main.c
//  game
//
//  Created by Daniel Szydłowski on 14/08/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef enum{
  playerError = -3,
  columnFull = -2,
  incorrectColumnNumber = -1,
  PawnPutCorrectly = 0,
} BoardErrors;

typedef enum{
  empty = 0,
  X,
  O,
  Xblink,
  Oblink
}Pawns;

static Pawns board[7][6];

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

/* Use this function to inform player that choosen column is full.
 All pawns in column passed in argument will be set to blinking.
 Function returns error if selected column is not full.
 */
void SetColumnAsBlinking(int column)
{
  int i = 0;
  for (i = 0; i < 7; i++)
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


/* Function puts user's pown on selected column. Function responsible for
 simulation of gravity - that means pawn is always put on possible lowest level.
 RETURNS:
  0 (PawnPutCorrectly) - pawn putted on board correctly
 -1 (incorrectColumnNumber) - returned if argument 'column' is higher than 6 or lower than 0,
 -2 (columnFull) - returned if column is full and it is not possible to put any more pawns
 -3 (playerError) - returned if player id passed as an argument is lower than 1 or
    higher than 2 */
int PutPownOnBoard (int column, Pawns player)
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

int Analize(void)
{
  int wiersz = 0;
  int kolumna = 0;
  
  int i, j;
  int gameFinished = 0;
  int player;
  int fieldsFound = 0;
  
  for (wiersz = 0; wiersz < 6 && gameFinished == 0; wiersz++)
  {
    for (kolumna = 0; kolumna < 7 && gameFinished == 0; kolumna++)
    {
      if (board[kolumna][wiersz] == 1 || board[kolumna][wiersz] == 2)
      {
        player = board[kolumna][wiersz];
        
        /* sprawdzanie w poziomie w prawo */
        for (i = kolumna+1; i < 7; i++)
        {
          /* nastepne field nie nalezy do tego gracza */
          if (board[i][wiersz] != player)
          {
            break;
          }
          else
          {
            fieldsFound++;
          }
          
          if (fieldsFound >= 3)
          {
            gameFinished = 1;
            board[i][wiersz] += 2;
            board[i-1][wiersz] += 2;
            board[i-2][wiersz] += 2;
            board[i-3][wiersz] += 2;
            break;
          }
        }
        fieldsFound = 0;
        /* sprawdzanie po skosie */
        for (i = kolumna+1, j = wiersz+1; i < 7 && j < 6 && gameFinished != 1; i++, j++)
        {
          if(board [i][j] != player)
          {
            fieldsFound = 0;
            break;
          }
          else
          {
            fieldsFound++;
          }
          
          if (fieldsFound >= 3)
          {
            gameFinished = 1;
            board[i][j] += 2;
            board[i-1][j-1] += 2;
            board[i-2][j-2] += 2;
            board[i-3][j-3] += 2;
          }
        }
        fieldsFound = 0;
        for (i = kolumna+1, j = wiersz-1; i < 7 && j >= 0 && gameFinished != 1; i ++, j--)
        {
          if(board [i][j] != player)
          {
            break;
          }
          else
          {
            fieldsFound++;
          }
          
          if (fieldsFound >= 3)
          {
            gameFinished = 1;
            board[i][j] += 2;
            board[i-1][j+1] += 2;
            board[i-2][j+2] += 2;
            board[i-3][j+3] += 2;
          }
        }
        fieldsFound = 0;
        for (j = wiersz +1; j< 6 && gameFinished == 0; j++)
        {
          if (board[kolumna][j] != player)
          {
            break;
          }
          else
          {
            fieldsFound++;
          }
          
          if (fieldsFound >= 3)
          {
            gameFinished = 1;
            board[kolumna][j] += 2;
            board[kolumna][j-1] += 2;
            board[kolumna][j-2] += 2;
            board[kolumna][j-3] += 2;
          }
        }
        fieldsFound = 0;
      }
    }
  }
  
  return gameFinished;
}

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
  int leftSide;
  int rightSide;
  int leftCol;
  int rightCol;
}T_Line;

void searchLines (Pawns player)
{
  int row, col, i;
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
          if (col +1 < 7 && row-1 > 0 && board[col+1][row-1] != player)
          {
            /* new line found, create object for it */
            newLine = (T_Line*) malloc(sizeof (T_Line));
            newLine->col = col;
            newLine->row = row;
            newLine->direction = LEFT_UP;
            newLine->pawnsNumber = 2;
            
            /* Check if field before is available */
            
            
            for (i = 2; i < 6; i++)
            {
              if (col -i < 0 || row+i > 6 )
              {
                /* board border reached - line locked on this side */
                newLine->leftSide = -1;
                newLine->leftCol = -1;
                break;
              }
              else if (board [col -i][row+i] != player && board [col -i][row+i] != empty)
              {
                /* field on board has been taken by enemy - line locked on this side */
                newLine->leftCol = col-i;
                newLine->leftSide = -1;
                break;
              }
              else if (board[col-2][row+2] == player)
              {
                /* next pawn has been found */
                newLine->pawnsNumber++;
              }
              else if (board [col -i][row+i] == empty)
              {
                newLine->
              }
            }
            
          }
          else
          {
            /* This pawn is in the middle of another line - new line not created */
          }
        }
        
        
        
        
      }
    }
  }
}


char algorithm(void)
{
  /* szukanie lini przeciwnika */
  /* szukanie wlasnych linii */
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

  
  return 'q';
}

int main()
{
  Pawns player = X;
  int column;
  char command;
  BoardErrors result;
  int gameFinished = 0;
  int numberOfPlayers = 0;
  
  while ( 1 )
  {
    if (numberOfPlayers == 0)
    {
      printf("\e[1;1H\e[2J"); /* clear screen */
      printf("Number of players (1/2): ");
      scanf("%d", &numberOfPlayers);
      DrawBoard();
      printf ("player \e[32mX\x1b[0m : ");
    }
    
    if (numberOfPlayers == 1 && player == O)
    {
      command = algorithm();
    }
    else if (numberOfPlayers == 2)
    {
      command = getchar();
    }
    else
    {
      printf ("main: Number of players incorrect!\n ");
      exit(0);
    }
    
    if (command >= '0' && command <= '9' && gameFinished == 0)
    {
      /* converting char number into int number: */
      column = command - '0';
      result = PutPownOnBoard(column, player);
      
      if (result == columnFull)
      {
        SetColumnAsBlinking(column);
        DrawBoard();
        printf("\a\e[31m KOLUMNA PELNA\n\e[0m");
      }
      else if (result == incorrectColumnNumber)
      {
        DrawBoard();
        printf("\a\e[5m\e[31m!!!\e[25m NIEMA TAKIEJ KOLUMNY \e[5m!!!\n\e[0m\e[25m");
      }
      else if (result == PawnPutCorrectly)
      {
        if (Analize() == 1)
        {
          DrawBoard();
          printf("gra skonczona! [r]estart [q]uit\n");
          gameFinished = 1;
          continue;
        }
        /* switch to another player */
        if (player == X)
        {
          player = O;
        }
        else
        {
          player = X;
        }
        DrawBoard();
      }
      
      if (player == X)
      {
        printf ("player \e[32mX\x1b[0m : ");
      }
      else
      {
        printf ("player  \x1b[31mO\x1b[0m : ");
      }
    }
    else if (command == 'q' || command == 'Q')
    {
      printf ("Gra zakonczona!\n");
      break;
    }
    else if (command == 'r' || command == 'R')
    {
      ClearArea();
      DrawBoard();
      gameFinished = 0;
      numberOfPlayers = 0;
    }
  }
  return 0;
}

