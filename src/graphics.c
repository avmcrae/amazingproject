/*  graphics.c  C file for ASCII graphics portion of Amazing project

    Project name: Amazing Project
    Component name: Graphics

    This file contains all associated functions for the display of 
    the maze view to the terminal using ASCII representation. The 
    graphical display relies on the NCURSES library. The primary
    function is drawMaze, and all associated functions help in this
    process except for initGraphics.
    
    Team: Raboso
    Authors: Avalon McRae, Ted Owens, Daniel Chen
    Last Updated: 5/27/14

    Special considerations:  compile with ncurses library.
    
======================================================================*/
// See header file for other includes and items
//
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>

// ---------------- Local includes  e.g., "file.h"
    #include "graphics.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 

// ---------------- Private variables 

    // Tracks where the upper right corner of the maze should
    // be placed on the terminal display.
    int originX;
    int originY;

// ---------------- Private prototypes 

/*====================================================================*/


/* |--initGraphics--| Figures out what to treat as the origin of the maze on the
 * terminal window so that the maze is centered properly.
 */
void initGraphics()
{
    /* Calculating for a center placement */
    originY = (LINES -  2 * mazeHeight) / 2; 
    originX = (COLS - 2 * mazeWidth) / 2;
}

/* |--drawMaze--| Draws the entire maze to the terminal window. initGraphics 
 * must be called before drawMaze. drawMaze is called after every move by an 
 * avatar. It draws all of the walls that are known to the avatars shared 
 * memory, as well as the positions of the avatars, and the position of the 
 * goals.
 */
int drawMaze()
{
    //-----------------------------DRAW THE SQUARES---------------------------//
    // Clear the terminal window
    clear();

    // Iterators for rows and columns
    int i,j;

    // For every column
    for (i = 0; i < mazeWidth; i++)
    {
        // For every row
        for (j = 0; j < mazeHeight; j++)
        {
            drawMazeSquare(i, j);
        }
    }

    //-----------------------------DRAW THE BORDERS---------------------------//

    // Top border
    move(originY - 1, originX);
    for (i = 0; i < mazeWidth * 2; i++)
    {
        addch('-');
    }

    // Bottom border
    move (originY + mazeHeight * 2 - 1, originX);
    for (i = 0; i < mazeWidth * 2; i++)
    {
        addch('-');
    }

    // Left border
    for (i = 0; i < mazeHeight * 2; i++)
    {
        move(originY + i, originX - 1);
        addch('|');

    }

    // Right border
    for (i = 0; i < mazeHeight * 2; i++)
    {
        move(originY + i, originX + mazeWidth * 2 - 1);
        addch('|');
    }
 

    // Iterate through the avatars
    for (i = 0; i < avatarCount; i++)
    {
        InformedAvatar *current = AvatarList[i];
        int x = current->currentposition.x;
        int y = current->currentposition.y;
        move(originY + y * 2, originX + x * 2);
        printw("%d", i);
    }

    //-----------------------------DRAW THE GOAL------------------------------//

    move(originY + sharedGoal->y * 2, originX + sharedGoal->x * 2);
    printw("G");
    move(originY,originX);

    refresh();
    return 1;
}

/* |--drawMazeSquare--| Draw an individual position in the maze. This mostly
 * involves printing the walls, if they exist near that position.
 */
int drawMazeSquare(int i, int j)
{
    move(originY + 2 * j, originX +  2 * i + 1); /* reverse i and j */

    // Check for right wall
    if (hasRightWall(i, j))
    {
        addch('|');
    }

    // Check for bottom wall
    move (originY + 2 * j + 1, originX + 2 * i);
    if (hasBottomWall(i, j))
    {
        addch('-');
    }
    else 
    {
        addch(' ');
    }

    // Add the corner
    addch('+');

    return 1;
}

/* |--hasRightWall--| Checks in the shared memory whether or not the maze
 * square has a right wall.
 */
int hasRightWall(int i, int j)
{
    MazeSquare *square = SharedMazeView[i][j];
    return square->walls[3];
}

/* |--hasBottomWall--| Checks in the shared memory whether or not the maze
 * square has a bottom wall.
 */
int hasBottomWall(int i, int j)
{
    MazeSquare *square = SharedMazeView[i][j];
    return square->walls[2];
}


