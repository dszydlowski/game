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
#include "algorithm.h"
#include "board.h"


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

int main()
{
  Pawns player = X;
  int column;
  char command;
  BoardErrors result;
  int gameFinished = 0;
  int numberOfPlayers = 0;
  
  srand((unsigned int) time(NULL));

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
      command = Algorithm();
      sleep(2);
    }
    else if (numberOfPlayers == 2 || (numberOfPlayers == 1 && player == X) )
    {
      command = getchar();
      if (command == '\n')
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
          printf("gra skonczona! Zwyciezyl gracz ");
          if (player == X)
            printf ("\e[32mX\x1b[0m\n");
          else
            printf ("\x1b[31mO\x1b[0m\n");
          printf ("[r]estart [q]uit\n");
          gameFinished = 1;
          player = X;
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
      //printf ("Gra przerwana!\n");
      break;
    }
    else if (command == 'r' || command == 'R')
    {
      ClearArea();
      DrawBoard();
      gameFinished = 0;
      numberOfPlayers = 0;
      player = X;
    }
  }
  return 0;
//  DrawBoard();
//  Algorithm();
}

