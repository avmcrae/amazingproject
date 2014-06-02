/* ========================================================================== */
/* File: avatar.c
 *
 * Project Name: Amazing Project
 *
 * Author: Avalon McRae, Ted Owens, Daniel Chen
 * Date: 5/22/14
 *
 * Command line options: None
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>	   
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
#include <string.h>
#include <getopt.h>	  // Getopt
#include <sys/socket.h>     // Socket function
#include <sys/types.h>      // Sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <math.h> 
#include <ncurses.h>


// ---------------- Local includes  e.g., "file.h"
#include "avatar.h"
#include "graphics.h"

// ---------------- Constant definitions

// ---------------- Macro definitions
#define TRUE 1
#define FALSE 0
// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/* ========================================================================== */

/*
	Function that starts up an avatar.
	Takes as a parameter the thread input.
	Has a loop that continually checks for the server message and then acts on 
	the results of that message.
*/
void *avatar(void *threadInput)
{
	/* Initialize variables to store the current thread's avatar info */
	int ID;
	int CurrentAvatar;
	int atGoal = 0; // Change to 1 as soon as the avatar reaches the goal

	// boolean to determine when to cut off maze
	MazeInProgress = TRUE;

	// used to keep track of the previuos move
	// ONLY updated when the avatar actually moves from one place to another
	int previousMove = -1;
	
	// Get the avatar start-up information
	AvatarInit *newAvatar = ((AvatarInit *) threadInput);
	ID = newAvatar->AvatarId;
	
	//--------------------------CONNECT TO SERVER-----------------------------//
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	fputs("Error creating TCP socket.", stderr);
	exit(2);
	}

	struct sockaddr_in servaddr;	
	memset(&servaddr, 0, sizeof(servaddr));

	//---------------------------CONNECT TO SERVER----------------------------//
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(newAvatar->serverIP);
	servaddr.sin_port = htons(newAvatar->MazePort);
	
	if ((connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0){
	fputs("Error establishing a connection with TCP server.", stderr);
	exit(3);
  	}

	//-------------------------SEND AVATAR READY------------------------------//
	AM_Message *AvMessage;
	AvMessage = calloc(1, sizeof(AM_Message));
	
	AvMessage->type = htonl(AM_AVATAR_READY);
	AvMessage->avatar_ready.AvatarId = htonl(newAvatar->AvatarId);
	
	send(sockfd, AvMessage, sizeof(AM_Message), 0);

	//------------------------CHECK RETURN MESSAGE----------------------------//
	AM_Message *serverMessage;
	serverMessage = calloc(1, sizeof(AM_Message));
	
	// Listen for received message
	recv(sockfd, serverMessage, sizeof(AM_Message), 0);
	
	// Get the return type
	uint32_t avReadyReturnVal = ntohl(serverMessage->type);
	
	// Check for error
	if (avReadyReturnVal != AM_AVATAR_TURN)
	{
	fprintf(stderr, "Server is not ready for avatars to make turns\n");
	return NULL;
	}
  
  	// For the first avatar,
  	if ( serverMessage->avatar_turn.TurnId == ID )
  	{
		// print starting locations
		if (!GRAPHICS)
		{
		  	printf("Printing initial avatar locations:\n");
		  
		  	for ( int i = 0; i < numAvatars; i++ )
		  	{
		  		printf("Avatar %d: (%d,%d)\n", i, 
				ntohl(serverMessage->avatar_turn.Pos[i].x),
				ntohl(serverMessage->avatar_turn.Pos[i].y));
			
				SharedMazeView[ntohl(serverMessage->avatar_turn.Pos[i].x)]
				[ntohl(serverMessage->avatar_turn.Pos[i].y)]->occupied++;
			}
		}
		setGoalLocation(serverMessage);
	}
  
	//------------------ESTABLISH DATA STORAGE FOR AVATAR--------------------//
	  
	// Allocate the avatar
	InformedAvatar *av;
	av = calloc(1, sizeof(InformedAvatar));
	av->currentposition = serverMessage->avatar_turn.Pos[ID];
	av->lastPositionDeadEnd = 0;
	
	// add avatar to AvatarList
	AvatarList[ID] = av;
	
	// store current position
	av->currentposition.x = ntohl(serverMessage->avatar_turn.Pos[ID].x);
	av->currentposition.y = ntohl(serverMessage->avatar_turn.Pos[ID].y);
	
	sleep(1);
		
	//------------------------ENTER TURN BY TURN LOOP-------------------------//
	
	int move;
	
	while (MazeInProgress)
	{	
		// Get the location of the avatar
		av->currentposition.x = ntohl(serverMessage->avatar_turn.Pos[ID].x);
		av->currentposition.y = ntohl(serverMessage->avatar_turn.Pos[ID].y);
		
		// Check whose turn it is
		CurrentAvatar = ntohl(serverMessage->avatar_turn.TurnId);
		
		// If it is this avatar's turn
		if (ID == CurrentAvatar)
		{
			// do this is the avatar has not been previously shown to be at
			// goal
	  		if (!atGoal)
			{
				// if avatar is at goal
				if ( goalTest(av->currentposition.x, av->currentposition.y) )
				{
		  			// print that avatar is at goal
					if (!GRAPHICS)
					{
						printf("AVATAR %d AT GOAL. Not moving.\n", ID);
					}
		  			
		  			// turn on atGoal
					move = M_NULL_MOVE;
					atGoal = 1;
				}

				// avatar is not at goal
				else
				{
					// get the next move
					move = getNextMove(SharedMazeView, av, previousMove);
				}
		
				// print stuff if graphics is not on
				if (!GRAPHICS)
				{
					printf("Avatar %d, moving %d from (%d, %d).", ID, move, 
				  	av->currentposition.x, av->currentposition.y);	
				  	
				  	if (av->lastPositionDeadEnd)
					{
						printf(" Creating dead end.\n");
					}
				  	else
					{
						printf("\n");
					}				
				}
				
				// for graphics
				else
				{
				  drawMaze();
				}		
			}
	  
			/* Send the Avatar's move to the server */
			AvMessage->type = htonl(AM_AVATAR_MOVE);
			AvMessage->avatar_move.AvatarId = htonl(ID);
			AvMessage->avatar_move.Direction = htonl(move);
			  
			send(sockfd, AvMessage, sizeof(AM_Message), 0);	  		
	  
		}
	
		/* Keep checking message from the server */
		recv(sockfd, serverMessage, sizeof(AM_Message), 0);
		
		/* If the server timed out, inform the user and exit */
		if (ntohl(serverMessage->type) == AM_SERVER_TIMEOUT) 
		{
			fprintf(stderr, "ERROR: There was a server timeout.");
			exit(1);
		}
		
		/* If disk quota was reached, inform the user that information
		* can no longer be written to the log file and saved */
		if (ntohl(serverMessage->type) == AM_SERVER_DISK_QUOTA) 
		{
			fprintf(stderr, "ERROR: You have exceeded your disk quota. Information may be missing from the logfile");
		}
		
		/* If the server is out of memory, inform the user and exit */
		if (ntohl(serverMessage->type) == AM_SERVER_OUT_OF_MEM) 
		{
			fprintf(stderr, "ERROR: The server is out of memory");
			exit(2);
		}
		
		/* Check for any other miscellaneous errors */
		if (IS_AM_ERROR(serverMessage->type))
		{
			fprintf(stderr, "ERROR: %d\n", serverMessage->type);
			break;
		}
		
		/* If the maze is solved, print results to log file and exit loop */
		if (ntohl(serverMessage->type) == AM_MAZE_SOLVED) {
		
			/* Only have one Avatar write final info to log file */
			if (ID == 0)
			{
				/* Print final info to logfile, i.e. the hash from the server*/
				FILE *fp;
				fp = fopen(newAvatar->logFile, "a");
				if (fp != NULL) {
				fprintf(fp, "%s %d %s\n", "Avatar", ID, "writes to log file:");
				
				uint32_t moves = ntohl(serverMessage->maze_solved.nMoves);
				fprintf(fp, "%s %d %s\n", "Congratulations! You have solved the maze in", moves, "moves!");
				
				uint32_t hash = ntohl(serverMessage->maze_solved.Hash);
				fprintf(fp, "%s %d\n", "The Hash from the AM_MAZE_SOLVED message is:", hash);
				
				fclose(fp);
				}
					
				/* Close graphics if we are using it */
				if (GRAPHICS)
				{
				sleep(5);
				clear();
				refresh();
				endwin();
				}
				printf("FINISHED MAZE.\n");
			
				/* If the maze is complete, exit the while loop that is running the maze*/
				MazeInProgress = 0;
			}
		}

		else
		{
			// if it was just this avatar's move
			if (ID == CurrentAvatar && move != M_NULL_MOVE )
		    {
				/* Update MazeSquare Occupancy */
				SharedMazeView[av->currentposition.x][av->currentposition.y]->
				occupied--;
				SharedMazeView[ntohl(serverMessage->avatar_turn.Pos[ID].x)]
				[ntohl(serverMessage->avatar_turn.Pos[ID].y)]->occupied++;
				
				/* If the last position was a dead end */
				if (av->lastPositionDeadEnd)
				{
				  	// If no other avatars are occupying the previous space
				  	// as well as the current space
				  	if (! SharedMazeView[av->currentposition.x]
				  		[av->currentposition.y]->occupied &&
						! (SharedMazeView[ntohl(serverMessage->
							avatar_turn.Pos[ID].x)]
						[ntohl(serverMessage->avatar_turn.Pos[ID].y)]-1))
				    {
					addWall(SharedMazeView, move, av->currentposition.x, 
						av->currentposition.y);
				    }
				  
				  	// set current position to not dead end
				  	av->lastPositionDeadEnd = 0;
				  
				  	// don't do the opposite of the move just done
				  	previousMove = move;
				}
				
				// if the avatar did not move from the last position
				else if ( av->currentposition.x 
					== ntohl(serverMessage->avatar_turn.Pos[ID].x) && 
					av->currentposition.y 
					== ntohl(serverMessage->avatar_turn.Pos[ID].y))
				{
				  	// add a wall where the avatar just tried to move
				  	addWall(SharedMazeView, move, av->currentposition.x, 
					av->currentposition.y);
				}
				
				// the avatar moved normally
				else
				{
				  // don't do the opposite of the move just done
				  previousMove = move;
				}
		    }		
		}
  	}
	/* Free all AM_Message structs allocated in the function */
	free(AvMessage);
	free(serverMessage);

	/* Free thread info/avatar structs */
	free(threadInput);
	free (av);
	  
	return NULL;
}

/*
	Returns the move that an avatar should make. Takes in the mazeview, the 
	avatar struct, and the avatar's previous move.
	Retrieves wall information from maze view, evaluates moves based on the
	walls and the heuristic, and then returns the best move.
*/
int getNextMove(MazeSquare ***mazeView, InformedAvatar *avatar, int previousMove)
{
	// get possible moves from mazeview
	int *possibleMoves 
		= getPossibleMoves(mazeView[avatar->currentposition.x]
		[avatar->currentposition.y]);

	// variables used to find best move
	long bestValue = 2147483646;
	long tempValue = 0;
	int bestMove = -1;
	
	long nextBestValue = 2147483646;
	int nextBestMove = -1;

	// loop through all of the possible moves
	for (int move = 0; move < M_NUM_DIRECTIONS; move++)
	{
		// if the move is possible
		if ( possibleMoves[move] == 0 )
		{
			tempValue = heuristic(move, avatar->currentposition.x, avatar->currentposition.y, previousMove);
			
			// if the move would be the best
			if ( tempValue < bestValue )
			{
				// set the current best move as the second best move
				nextBestMove = bestMove;
				nextBestValue = bestValue;

				// set the new best move
				bestMove = move;
				bestValue = tempValue;
			}

			// if the move would be the second best
			else if ( tempValue < nextBestValue )
			{
				// set the next best move
				nextBestValue = tempValue;
				nextBestMove = move;
			}

		}
	}

	// if there is no "next best move"
	if ( nextBestMove == -1 )
	{
		// means there is only one option, so this is a deadend
		avatar->lastPositionDeadEnd = 1;
	} 

	// if the best move is the opposite of the previous move,
	// and there is a second move that can be used
	else if ( getOppositeMove(bestMove) == previousMove )
	{
		return nextBestMove;
	}

	return bestMove;
}

/*
	Heuristic that is used to prioritize potential moves for an avatar.
	Priorities are ordered from 1 to 4, 1 is first priority.
	Takes as parameters the move to evaluate, the avatar's current position in 
	x and y coorindates, and the avatar's previous move.
	If previousMove is -1, a manhattan distance heuristic is used.
	If previousMove is not, moves are ordered in priority of right turn, straight, 
	left, then backwards.
*/
long heuristic(int move, int currentX, int currentY, int previousMove)
{
	if ( previousMove == -1 )
	{
		long x = currentX;
		long y = currentY;

		switch ( move ) 
		{
			case M_WEST:
				x--;
			  	break;

			case M_NORTH:
			  	y--;
			  	break;

			case M_SOUTH:
			  	y++;
				break;

			case M_EAST:
			  	x++;
				break;

			default:
				fprintf(stderr, "Heuristic received incorrect move %d\n", move);
				return 2147483646;
			  	break;
		}

		x = powl(x - sharedGoal->x, 2);
		y = powl(y - sharedGoal->y, 2);

		return x + y;		
	}

	// second highest priority is straight
	if ( move == previousMove)
	{
		return 2;
	}

	// last priority is backwards
	else if ( move == getOppositeMove(previousMove))
	{
		return 4;
	}

	// switches to determine if move is east or west of current
	switch ( previousMove ) 
	{
		case M_WEST:

			if ( move == M_NORTH)
			{
				return 1;
			}
			else
			{
				return 3;
			}

		  	break;

		case M_NORTH:

			if ( move == M_EAST)
			{
				return 1;
			}
			else
			{
				return 3;
			}

		  	break;

		case M_SOUTH:

			if ( move == M_WEST)
			{
				return 1;
			}
			else
			{
				return 3;
			}

			break;

		case M_EAST:

			if ( move == M_SOUTH)
			{
				return 1;
			}
			else
			{
				return 3;
			}

			break;

		default:
			fprintf(stderr, "Heuristic received incorrect move combination %d\n", move);
			return 2147483646;
		  	break;
	}

	return 5;
}

/*
	Helper function the returns the opposite of the move given in the parameter.
	Returns -1 if move given is invalid.
*/
int getOppositeMove(int move)
{
	switch ( move ) 
	{
		case M_WEST:
			return M_EAST;

		case M_NORTH:
		  	return M_SOUTH;

		case M_SOUTH:
		  	return M_NORTH;

		case M_EAST:
		  	return M_WEST;

		default:
			fprintf(stderr, "getOppositeMove received incorrect move %d\n", move);
			return -1;
	}
}

/*
	Simple test to see if the avatar is on the goal.
	Takes as parameter the x and y position of the avatar.
	returns 1 if at goal, 0 otherwise.
*/
int goalTest(int x, int y)
{
	return x == sharedGoal->x && y == sharedGoal->y;
}

