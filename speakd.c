/**
 ** Charlie Lees CS5700
 ** CS5700 HW1 Speakd
 ** Communicates over TCP with Speak.c
 **
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "speakd.h"
#include <errno.h>

extern char *inet_ntoa( struct in_addr );

#define NAMESIZE		255
#define BUFSIZE			81
#define listening_depth		2

/*
 * sendMode - takes in a message from the user and sends it to the connected port
 * fd - port that we are communicating with speak.c
 */

int sendMode(short fd){
	int c, i, n, len;
	ssize_t ackLen;//Stores length of acknowledgement Message
	char buffer[BUFSIZE];//Stores message sent
	char buffer2[BUFSIZE];//Stores ack 
	//User Prompt
	fputs("**********Entering Send Mode**********\n", stdout);
	fputs("Enter 'x' to switch modes, 'xx' to quit\n", stdout);
	fputs("Enter message to send: ", stdout);
while( (fgets( buffer, BUFSIZE, stdin ) != NULL))
	{	
	
	len = strlen(buffer);
	
	//Pre-send modifications go here	
	
	if( (n = send(fd, buffer, len, 0)) != len )
		if( n < 0 )
			{
			perror( "speakd send" );
			exit(1);
			}
		else
			{
			fprintf(stderr, "speakd sent %d bytes, attempted %d\n", n, len);
			break;
			}

	
	
	//After send modifications go here
	//Receiving Acknowledgement
	ackLen = recv(fd, buffer2, BUFSIZE, 0);
	buffer2[ackLen] = '\0';
	if(ackLen > 0){
		//Acknowledgement Received
		if(strcmp(buffer2, "Message Received") == 0){
			//Print message for successful message	
			//Adding spaces to the print for readablility
			printf("     Acknowledgement Received\n");
		}
		//Received Error from Listener
		else{
			printf("speak.c error: %s\n", buffer2);	
			return -1;
		}
	}
	else{
		//If there was an error receiving anything,
		//Print error and end program
		fprintf(stderr, "speakd.c %s\n", strerror(errno));
		return -1;
	}
	//If non-special value, prompt user for input
	if(strcmp(buffer,"x\n\0")!=0&&strcmp(buffer, "xx\n\0"))
	{
		fputs("Enter message to send: ", stdout);

	}	
	//If we sent out x
	if(strcmp(buffer, "x\n\0") == 0){
		//Return to other mode
		return 0;
	}
	
	//If we sent out xx
	if(strcmp(buffer, "xx\n\0") == 0){
		//Prints feedback
		fputs("Quit Value Given \n", stdout);
		close(fd);//closes port
		exit(0);//terminates program

	}
	}
	return -1;
}



void speakd( int server_number )
{
int			c, i;
int			n, len;
short			fd, client_fd;
struct sockaddr_in	address, client;
struct hostent		*node_ptr;
char			local_node[NAMESIZE];
char			buffer[BUFSIZE+1];

//ASCII Header
printf("███████ ██████  ███████  █████  ██   ██ ██████\n");
printf("██      ██   ██ ██      ██   ██ ██  ██  ██   ██\n");
printf("███████ ██████  █████   ███████ █████   ██   ██\n");
printf("     ██ ██      ██      ██   ██ ██  ██  ██   ██\n");
printf("███████ ██      ███████ ██   ██ ██   ██ ██████\n");
printf("------------------------------------------------------------\n");
printf("------------------------------------------------------------\n");

/*  get the internet name of the local host node on which we are running  */
if( gethostname( local_node, NAMESIZE ) < 0 )
	{
	perror( "speakd gethostname" );
	exit(1);
	}
fprintf(stderr, "speakd running on node %s\n", local_node);

/*  get structure for local host node on which server resides  */
if( (node_ptr = gethostbyname( local_node )) == NULL )
	{
	perror( "speakd gethostbyname" );
	exit(1);
	}

/*  set up Internet address structure for the server  */
memset(&address, 0, sizeof(address));
address.sin_family = AF_INET;
memcpy(&address.sin_addr, node_ptr->h_addr, node_ptr->h_length);
address.sin_port = htons(server_number);

fprintf(stderr, "speakd full name of server node %s, internet address %s\n",
		node_ptr->h_name, inet_ntoa(address.sin_addr));

/*  open an Internet tcp socket  */
if( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
	perror( "speakd socket" );
	exit(1);
	}

/*  bind this socket to the server's Internet address  */
if( bind( fd, (struct sockaddr *)&address, sizeof(address) ) < 0 )
	{
	perror( "speakd bind" );
	exit(1);
	}

/*  now find out what local port number was assigned to this server  */
len = sizeof(address);
if( getsockname( fd, (struct sockaddr *)&address, &len ) < 0 )
	{
	perror( "speakd getsockname" );
	exit(1);
	}

/*  we are now successfully established as a server  */
fprintf(stderr, "speakd at internet address %s, port %d\n",
		inet_ntoa(address.sin_addr), ntohs(address.sin_port));

/*  start listening for connect requests from clients  */
if( listen( fd, listening_depth ) < 0 )
	{
	perror( "speakd listen" );
	exit(1);
	}

/*  now accept a client connection (we'll block until one arrives)  */
len = sizeof(client);
if( (client_fd = accept(fd, (struct sockaddr *)&client, &len)) < 0 )
	{
	perror( "speakd accept" );
	exit(1);
	}

/*  we are now successfully connected to a remote client  */
fprintf(stderr, "speakd connected to client at Internet address %s, port %d\n",
		inet_ntoa(client.sin_addr), ntohs(client.sin_port));

printf("------------------------------------------------------------\n");
//Initial User prompt
fputs("Waiting for message from connection...\n", stdout);
//Receiving message from connection
while( (n = recv( client_fd, buffer, BUFSIZE, 0)) > 0 )
	{	
	//Sending Acknowledgement
	send(client_fd, "Message Received", strlen("Message Received"), 0);	
	//Places terminal value at the end of the string
	buffer[n] = '\0';
	n--;
	//Makes sure to not touch carriage return
	if( buffer[n] == '\n' )
		n--;
	

	//If receives x
	if(strcmp(buffer, "x\n\0")==0){
		//sendMode(client_fd);
		//no errors in send mode
		if(sendMode(client_fd) == 0){
			fputs("**********Returning to Receive Mode**********\n", stdout);
			fputs("Waiting for message from connection...\n", stdout);
		}
		//Errors
		else{
			//If sendMode returns a not 0
			//There was an error, ending program
			close(client_fd);
			close(fd);
			exit(0);	
		}


	}


	//If client enters xx
	if(strcmp(buffer, "xx\n\0")==0){
		

		//prints feedback to user that terminal value was given
		fputs("Quit Value Given \n", stdout);
		close(client_fd);//closes port with client
		close(fd);//closes listening port
		exit(0);//terminates program
	
	}


	//Printing received message
	if(strcmp(buffer, "x\n\0")!=0){	
		fprintf(stdout, "Received Message: %s", buffer);
	}
	//fprintf(stdout, "Received Message: %s", buffer);
	}

//If an error was thrown by recv()
if( n < 0 )
	perror( "speakd read" );
	//Generate Error
	//Send Error
	send(client_fd,strerror(errno), BUFSIZE, 0); 	
	//Print out error
	fputs(strerror(errno), stderr);
	

/*  close the connection to the client  */
if( close(client_fd) < 0 )
	{
	perror( "speakd close connection to client" );
	exit(1);
	}

/*  close the "listening post" socket by which server made connections  */
if( close(fd) < 0 )
	{
	perror( "speakd close" );
	exit(1);
	}
}

