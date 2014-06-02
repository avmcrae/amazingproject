/* 	avatar.h Header for avatar component of A-Mazing Problem 

	Class: cs50, Dr. Palmer

	Project name: Amazing
	Component name: Avatar

	This file contains ...
	
	Team: Raboso
	Members: Avalon McRae, Ted Owens, Daniel Chen
	Date Created: 5/20/14
	
======================================================================*/
// do not remove any of these sections, even if they are empty

#ifndef AVATAR_H
#define AVATAR_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "amazing.h"
#include "maze.h"
#include "graphics.h"

// ---------------- Constants

// ---------------- Structures/Types

typedef struct AvatarInit
{
	int AvatarId;
	int nAvatars;
	int Difficulty;
	char *serverIP;
	int MazePort;
	char *logFile;
  
} AvatarInit;

typedef struct InformedAvatar {
  XYPos lastposition; // probably unnecessary
  XYPos currentposition; 

  int lastPositionDeadEnd;

  // might not be needed
  int previousmove;    // Contains an integer (represented by the amazing.h macros)
                       // for the last move the avatar made (N, W, S or E).
} InformedAvatar;

// ---------------- Public Variables
InformedAvatar **AvatarList;

// ---------------- Prototypes/Macros

/*
	Function that starts up an avatar.
	Takes as a parameter the thread input.
	Has a loop that continually checks for the server message and then acts on the 
	results of that message.
*/
void *avatar(void *threadInput);

/*
	Heuristic that is used to prioritize potential moves for an avatar.
	Priorities are ordered from 1 to 4, 1 is first priority.
	Takes as parameters the move to evaluate, the avatar's current position in 
	x and y coorindates, and the avatar's previous move.
	If previousMove is -1, a manhattan distance heuristic is used.
	If previousMove is not, moves are ordered in priority of right turn, straight, 
	left, then backwards.
*/
long heuristic(int move, int currentX, int currentY, int previousMove);

/*
	Returns the move that an avatar should make. Takes in the mazeview, the 
	avatar struct, and the avatar's previous move.
	Retrieves wall information from maze view, evaluates moves based on the
	walls and the heuristic, and then returns the best move.
*/
int getNextMove(MazeSquare ***mazeView, InformedAvatar *avatar, int previousMove);

/*
	Helper function the returns the opposite of the move given in the parameter.
	Returns -1 if move given is invalid.
*/
int getOppositeMove(int move);

/*
	Simple test to see if the avatar is on the goal.
	Takes as parameter the x and y position of the avatar.
	returns 1 if at goal, 0 otherwise.
*/
int goalTest(int x, int y);

#endif // AVATAR_H

