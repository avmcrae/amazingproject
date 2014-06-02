/* ========================================================================== */
/* File: maze.c
 *
 * Project Name: Amazing Project
 * Component Name: maze.c
 * 
 * Team Name: Raboso
 * Authors: Avalon McRae, Daniel Chen, Ted Owens 
 * Last Updated: 5/27/2014
 *
 * This file contains the functions to initialize and update the MazeSquare struct that
 * tracks the maze. As the maze is traversed by the avatars, each maze square is 
 * updated as walls and dead ends are encountered.
 *
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>              
#include <stdlib.h>
#include <netinet/in.h>

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"                    //Direction macros and AM_Message struct
#include "maze.h"                       // Maze structs and function declarations
#include "graphics.h"                   //Graphics macro

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/* ========================================================================== */

/* Initialize a new Maze Square, setting all walls and dead ends to zero */
MazeSquare *initMazeSquare() {
  MazeSquare *newSquare = (MazeSquare *) calloc(1, sizeof(MazeSquare));

  /* Initialize an array for each of the 4 walls in a given square 
   * Set each element to zero because we don't know where any walls are yet */
  for (int i = 0; i < M_NUM_DIRECTIONS; i++) {
    newSquare->walls[i] = 0;
  }

  /* Avatars initial positions have yet to be processed, so all squares are 'empty' */
  newSquare->occupied = 0;

  return newSquare;
}

/* Create the goal location by getting each avatar's starting position from the server
 * and taking the average of all of the positions */
void setGoalLocation(AM_Message *message) {
  /* Create a new XYPos to store the goal location square 
   * This will be stored in a global variable so that each thread can access it */
  sharedGoal = (XYPos *) calloc (1, sizeof(XYPos));

  sharedGoal->x = 0;
  sharedGoal->y = 0;

  /* For each avatar, accumulate the avatar's x and y position */
  for (int i = 0; i < numAvatars; i++) 
  {
    sharedGoal->x += ntohl(message->avatar_turn.Pos[i].x);
    sharedGoal->y += ntohl(message->avatar_turn.Pos[i].y);
  }

  /* Take the average of the x and y positions */
  sharedGoal->x /= numAvatars;
  sharedGoal->y /= numAvatars;

  if (!GRAPHICS) {
    printf("Set sharedGoal location as (%d, %d)\n", sharedGoal->x,sharedGoal->y);
  }
  return;
}

/* Adds a Wall (by setting that wall's value to one in the maze square array.
 * Returns 1 if successful, 0 otherwise. */
int addWall(MazeSquare ***mazeview, int wall, int col, int row) {
  /* If the wall number input is a valid wall, set that element to one in the mazeview*/
  if (wall < M_NUM_DIRECTIONS) {
    mazeview[col][row]->walls[wall] = 1;
  }

  /* Otherwise, the input wall is out of bounds. Alert the user and return */
  else {
    fprintf(stderr, "Invalid wall given to addWall\n");
    return 0;
  }

  /* Add a wall to the neighboring square as well */
  switch (wall) 
  {
  case M_WEST:
    if (col - 1 >= 0) mazeview[col-1][row]->walls[M_EAST] = 1;
    break;
  case M_NORTH:
    if (row - 1 >= 0) mazeview[col][row-1]->walls[M_SOUTH] = 1;
    break;
  case M_EAST:
    if (col + 1 < mazeWidth) mazeview[col+1][row]->walls[M_WEST] = 1;
    break;
  case M_SOUTH:
    if (row + 1 < mazeHeight) mazeview[col][row+1]->walls[M_NORTH] = 1;
    break;
  default:
    return 0;
  }

  return 1;
}

/* This function returns a maze square's wall array. If an element is set to 1, it is a 
 * wall (not a possible move), if it is zero, it is still a possible move */
int* getPossibleMoves(MazeSquare* square) {
  return square->walls;
}

/* This function frees the Maze (stored as a pointer to a 2D array of MazeSquares) 
 * from memory */
void freeMaze(MazeSquare ***mazeview) {
  /* Look through each Maze Square, freeing each element of the 2D array*/
  int c;
  int r;
  for (c = 0; c < mazeWidth; c++) {
    for (r= 0; r < mazeHeight; r++) {
      free(mazeview[c][r]);
    }
  }

  /* Free each column of pointers */
  for (int i = 0; i < mazeWidth; i++) {
    free(mazeview[i]);
  }
  return;  
}
