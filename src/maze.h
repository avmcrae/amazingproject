/* ========================================================================== */
/* File: maze.h
 *
 * Project name: Amazing Project
 * Component name: Maze.h
 *
 * Team Name: Raboso
 * Authors: Avalon McRae, Daniel Chen, Ted Owens
 *
 * Last Updated: 5/27/14
 *
 * This file contains the defintion of the MazeSquare struct and the public variables
 * used to store maze info throughout the amazing_project. 
 */
/* ========================================================================== */
#ifndef MAZE_H
#define MAZE_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "amazing.h"   // Direction macros, i.e. M_NORTH, M_EAST, etc.

// ---------------- Constants

// ---------------- Structures/Types
typedef struct MazeSquare {	
	// see amazing.h for direction to index correspondence
	// 1 for wall, 0 if no wall or wall has yet to be discovered
	int walls[M_NUM_DIRECTIONS];

	int occupied;      // Initialized to zero. Set to 1 if it is a dead end.
} MazeSquare;

// ---------------- Public Variables
/* The pointer to a 2D array of Maze Squares, initialized in AMStartup. This can be
 * edited by each avatar thread to compile a comprehensive view of the maze */
MazeSquare ***SharedMazeView;

/* Stored the sharedGoal calculated in the setGoalLocation function */
XYPos *sharedGoal;

/* Global variables to store the maze info received from the server and user input */
int mazeWidth;
int mazeHeight;
int avatarCount;

// ---------------- Prototypes/Macros
/* Initialize a new Maze Square, setting all walls and dead ends to zero */
MazeSquare *initMazeSquare();

/* Create the goal location by getting each avatar's starting position from the server
 * and taking the average of all of the positions */
void setGoalLocation(AM_Message *message);

/* Adds a Wall (by setting that wall's value to one in the maze square array. 
 * Returns 1 if successful, 0 otherwise. */
int addWall(MazeSquare ***mazeview, int wall, int col, int row);

/* This function returns a maze square's wall array. If an element is set to 1, it is a  
 * wall (not a possible move), if it is zero, it is still a possible move */
int* getPossibleMoves(MazeSquare* square);

/* This function frees the Maze (stored as a pointer to a 2D array of MazeSquares)
 * from memory */
void freeMaze(MazeSquare ***mazeview);

#endif // MAZE_H
