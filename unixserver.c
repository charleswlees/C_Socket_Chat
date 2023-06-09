/*
 * Charlie Lees
 * CS5700 HW1
 * Unix Server Access Program
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "speakd.h"


#define default_server_number	0


int main( int argc, char*argv[] )
{
int	server_number;

/*  there must be zero or one command line argument  */
if( argc > 1 )
	{
	fprintf(stderr, "usage: speakd [server-number]\n");
	exit(1);
	}

/*  get the server's port number from the first parameter  */
server_number = default_server_number;

/*  now let the common server do the real work  */
speakd( server_number );

return(0);
}

