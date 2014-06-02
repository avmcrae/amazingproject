/* AMStartup.c
 *
 * Project name: Amazing Project
 * Component: AMStartup 
 *
 * Team Name: Raboso
 * Authors: Avalon McRae, Ted Owens, Daniel Chen
 *                  
 * This startup script is run at the beginning of the amazing_project. It takes all of
 * the intial maze arguments (number of avatars, difficulty level, and server hostname,
 * and initializes a connection with the server. It then takes information from the
 * server and stores the relevant maze initialization info in public variables and starts
 * running a thread for each avatar. At the end, it frees all user-allocated memory from
 * the maze.                                                      
 *
 * Compile: gcc -Wall -pedantic -std=c11 -lpthread -o start AMStartup.c
 */
/* ======================================================================*/
// ---------------- Open Issues                                        

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <unistd.h>         // Getopt
#include <getopt.h>         // Getopt
#include <stdlib.h>         // Exit Function
#include <sys/socket.h>     // Socket function
#include <sys/types.h>      // Sockets
#include <netinet/in.h>     // Server commmunication 
#include <arpa/inet.h>      // Server commmunication 
#include <netdb.h>          // Server commmunication
#include <pthread.h>        // Threads
#include <ncurses.h>        // Graphics
#include <time.h>           // Clock function

// ---------------- Local includes e.g., "file.h"
#include "amazing.h"
#include "avatar.h"
#include "maze.h"

// ---------------- Constant definitions                                   
     
// ---------------- Macro definitions

// ---------------- Structures/Types                                       
                
// ---------------- Private variables                                      
        
// ---------------- Private prototypes                                     
                
// ---------------- Public functions

int main(int argc, char *argv[]) {
  /* Initialize variables to store user input */
  int difficulty;
  char *hostname;
  hostname = NULL;
    
  char *programname;
  programname = argv[0];

  /* Check to ensure that the user input all 3 required switches. If not, inform the user
   * and exit */
  if (argc != 7) {
    fprintf(stderr, "%s\n", "You must enter all 3 switches, -n Number of Avatars, -d Difficulty, and -h hostname");
    exit(1);
  }

  /* Use getopt to get user input (number of avatars, difficulty, and hostname */
  int ch;
  while ((ch = getopt(argc, argv, "n:d:h:")) != -1) {
    switch (ch) {
    case 'n':
      numAvatars = atoi(optarg);
      avatarCount = numAvatars;
      if (numAvatars > AM_MAX_AVATAR) {
	fprintf(stderr, "%s %d%s\n", "You entered", numAvatars, " avatars. Please enter a number less than 10.");
	return 1;
      }
      break;
      
    case 'd':
      difficulty = atoi(optarg);
      if (difficulty > AM_MAX_DIFFICULTY) {
	fprintf(stderr,"%s %d%s\n", "You entered level", difficulty, " difficulty. Please enter a number less than 9.");
	return 1;
      } 
      break;
      
    case 'h':
      hostname = optarg;
      break;

    default:
      printf("Error: bad option \'%c\'\n", ch);
      printf("Usage:  %s [-n] [-d] [-h] \n", programname);
      exit(9);
      break;
    }
  }

  //---------------------------CREATE THE LOGFILE------------------------------// 
  /*Create the name of the logfile based on user input (Number of Avatars & difficulty)*/
  char filename[50];
  memset(filename, '\0', sizeof(filename));
  strcat(filename, "Amazing_Raboso_");
  
  char temp[2];
  sprintf(temp, "%d", numAvatars);

  strcat(filename, temp);
  strcat(filename, "_");

  sprintf(temp, "%d", difficulty);
  strcat(filename, temp);
  strcat(filename, ".log");

  FILE *fp;
  fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "%s\n", "Error: unable to create logfile.");
    exit(6);
  }
  /* Print intial log file info: team name and date/time */
  fprintf(fp, "%s\n", "This is a logfile for Group Raboso's Amazing Project.");

  time_t mytime;
  mytime = time(NULL);
  fprintf(fp, "%s %s", "Program started at:", ctime(&mytime));
  
  //-----------------------ESTABLISH TCP CLIENT_SIDE SOCKET-------------------------// 
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fputs("Error creating TCP socket.", stderr);
    exit(2);
  }

  //---------------------------GET IP ADDRESS------------------------------//
  struct hostent *host = gethostbyname(hostname);
  if (host == NULL) {
    fprintf(stderr, "%s\n", "Error, gethostbyname failed.");
    exit(8);
  }

  //---------------------------CONNECT TO SERVER------------------------------//
  /* Initialize a sockaddr_in struct to store all server info */
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));

  struct in_addr *address = (struct in_addr *)host->h_addr_list[0];
  servaddr.sin_addr.s_addr=inet_addr(inet_ntoa(* address));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(AM_SERVER_PORT));

  /* Send connection request. If failed, inform user and exit */
  if ((connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
    fputs("Error establishing a connection with TCP server.", stderr);
    exit(3);
  } 

  //--------------------------SEND INIT MESSAGE-------------------------------//
  AM_Message *message; 
  message = calloc(1, sizeof(AM_Message));
  
  message->type = htonl(AM_INIT);
  message->init.nAvatars = htonl(numAvatars);
  message->init.Difficulty = htonl(difficulty);

  send(sockfd, message, sizeof(AM_Message), 0); 
  
  //------------------------CHECK FOR INIT OK MESSAGE-------------------------//
  AM_Message *receivedmessage;
  receivedmessage = calloc(1, sizeof(AM_Message));
  recv(sockfd, receivedmessage, sizeof(AM_Message), 0);

  /* Check return message type. If not AM_INIT_OK, inform the user and exit */
  uint32_t initReturnVal = ntohl(receivedmessage->type);

  if (initReturnVal == AM_INIT_FAILED) {
    fprintf(stderr, "%s\n", "Maze Initialization Failed.");
    return 1;
  }

  if (initReturnVal != AM_INIT_OK) {
    fprintf(stderr, "%s\n", "Unknown Error: did not receive AM_INIT msg.");
    return 1;
  }

  //----------------PULL MAZE INFO OUT OF INIT OK MESSAGE---------------------//
  uint32_t mazePort = ntohl(receivedmessage->init_ok.MazePort);
  mazeWidth = ntohl(receivedmessage->init_ok.MazeWidth);
  mazeHeight = ntohl(receivedmessage->init_ok.MazeHeight);

  /* Store the mazePort received from the server in the logfile. 
   * Close the log file so it can be opened by the avatar threads */
  fprintf(fp, "%s %d\n", "The MazePort is:", mazePort);
  fclose(fp);

  //--------------------------SET UP THE MAZE---------------------------------//

  /* Allocate memory for the SharedMazeView (public variable in maze.h */
  SharedMazeView  = (MazeSquare ***)malloc(mazeWidth * sizeof(MazeSquare **));
  for (int i = 0; i < mazeWidth; i++) {
    SharedMazeView[i] = (MazeSquare **)malloc(mazeHeight * sizeof(MazeSquare*));
  }

  /* Initialize MazeSquares (each element of the array) */
  int x;
  int y; 
  for (x = 0; x < mazeWidth; x++)
  {
    for (y = 0; y < mazeHeight; y++)
    {
      SharedMazeView[x][y] = initMazeSquare();
    }
  }

  //----------------------------SET UP GRAPHICS-------------------------------//

  if (GRAPHICS)
  {
    initscr();
    initGraphics();
    clear();
    refresh();
  }


  //----------------------------SET UP AVATARS--------------------------------//
  int i;
  AvatarList = calloc(numAvatars, sizeof(InformedAvatar));
  pthread_t threadList[AM_MAX_AVATAR];

  for (i = 0; i < numAvatars; i++)
  {
    AvatarInit *newAvatar = calloc(1, sizeof(AvatarInit));

    newAvatar->AvatarId = i;

    newAvatar->Difficulty = difficulty;
    newAvatar->nAvatars = numAvatars;
    newAvatar->serverIP = inet_ntoa(* address);
    newAvatar->MazePort = mazePort;
    newAvatar->logFile = filename;

    /* Create a thread for each avatar */
    pthread_create(&threadList[i], NULL, avatar, newAvatar);
  }
  
  sleep(2);

  //--------------------------END AVATAR CREATION-----------------------------//
  /* Keep AMStartup running while the avatar threads are running */
  while (MazeInProgress) {
    sleep(1);
  }

  for (i = 0; i < numAvatars; i++)
  {
    pthread_join(threadList[i], NULL);
  }

  if (GRAPHICS)
  {
    delwin(stdscr);
    endwin();
  }

  /* Free the SharedMazeView elements */
  freeMaze(SharedMazeView);
  free(SharedMazeView);

  /* Free all messages in AMStartup */
  free(message);
  free(receivedmessage);

  /* Free memory used to store avatar and maze info */
  free(AvatarList);
  free(sharedGoal);

  /* Close MazePort and socket connections */
  close(mazePort);
  close(sockfd);

  return 0;
}
