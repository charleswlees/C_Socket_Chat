/*
 * Charlie Lees
 * CS5700 HW1
 * Unix Client Access Program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "speak.h"

#define default_server_number	233+1024

int main( int argc, char*argv[] )
{
int 	i;//Stores whether or not we prompted the user for port
char	*server_node;
int	server_number;
char	inputString[10];//Stores port input via prompt to user

i = 0;//Initialize variable i
/*  there must be one or two command line arguments  */
//If there is more than 3 arguments
if(argc>3){
	fprintf(stderr, "usage: speak server-number [server-node]\n");
	exit(1);
}
//If there are less than 2 args
if(argc<2){
		printf("Enter Server Port: ");//Prompts user
		fgets(inputString, 10, stdin);//Takes in input
		server_number = atoi(inputString);//Assigns value to server_number
		i++;//Marks that we have prompted the user
}

if(i == 0){//If User wasn't prompted, check for an argument
/*  get the server's port number from the first parameter  */
server_number = atoi(argv[1]);
}
/*  get the server's node name from the second parameter  */
if( argc <= 2 )
	server_node = NULL;
else
	server_node = argv[2];

/*  now let the common client do the real work  */
speak( server_number, server_node );
return(0);
}
