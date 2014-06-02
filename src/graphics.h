/* 	graphics.h Header for graphics portion of Amazing project.

	Project name: Amazing Project
	Component name: Graphics

	This file contains all functions related to the drawing of the maze
	to the terminal. The primary function is drawMaze, which in turn
	relies on drwawMazeSquare to draw each square in the maze. AMStartup
	must first call initGraphics to determine how to center the maze
	on the terminal window.

	Graphics may experience viewing issues on large sizes of the maze
	with large text font in terminal. We recommend reducing the font
	size to view larger mazes.
	
	Team: Raboso
	Authors: Ted Owens, Daniel Chen, Avalon McRae	
	Date Finalized: 5/27/14
	
==============================================================================*/


#ifndef GRAPHICS_H
#define GRAPHICS_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include <ncurses.h>		/* Primary graphics library for ASCII */
#include <stdlib.h>

// ---------------- Local Includes
#include "maze.h"
#include "avatar.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros
#define GRAPHICS 1 			/* 0: Displays maze with graphics */
							/* 1: Does not display graphics, prints moves */

/* |--drawMaze--| Draws the entire maze to the terminal window. initGraphics 
 * must be called before drawMaze. drawMaze is called after every move by an 
 * avatar. It draws all of the walls that are known to the avatars shared 
 * memory, as well as the positions of the avatars, and the position of the 
 * goals.
 */
int drawMaze();

/* |--drawMazeSquare--| Draw an individual position in the maze. This mostly
 * involves printing the walls, if they exist near that position.
 */
int drawMazeSquare(int i, int j);

/* |--hasRightWall--| Checks in the shared memory whether or not the maze
 * square has a right wall.
 */
int hasRightWall(int i, int j);

/* |--hasBottomWall--| Checks in the shared memory whether or not the maze
 * square has a bottom wall.
 */
int hasBottomWall(int i, int j);

/* |--initGraphics--| Figures out what to treat as the origin of the maze on the
 * terminal window so that the maze is centered properly.
 */
void initGraphics();

#endif // GRAPHICS_H
