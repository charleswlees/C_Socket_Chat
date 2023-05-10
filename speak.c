/**
 ** Charlie Lees CS5700
 ** HW 1 Speak
 ** Communicates with TCP to server program Speakd.c
 **
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "speak.h"
#include <errno.h>

extern char *inet_ntoa( struct in_addr );

#define NAMESIZE		255
#define BUFSIZE			81

/**
 * receiveMode - Listens to the connected port for a message
 * fd - Port we will be talking to the server with
 */
int receiveMode(short fd){
	//Re-initializing variables 
	int c, i, n, len;
		
	char buffer[BUFSIZE+1];//Stores received message
	
	//User prompt	
	fputs("**********Entering Receive Mode**********\n", stdout);
	fputs("Waiting for message from connection...\n", stdout);
	while( ((n=recv(fd, buffer, BUFSIZE, 0))>0)){
		//Sending Acknowledgement
		send(fd, "Message Received", strlen("Message Received"), 0);
		
	
	//Places terminal value at the end of the string
	buffer[n] = '\0';
	n--;
	//Makes sure to not touch carriage return
	if( buffer[n] == '\n' )
		n--;

	
	//If x is received
	if(strcmp(buffer, "x\n\0")==0){
		
		return 0;
	}
	
	//If xx is received
	if(strcmp(buffer, "xx\n\0")==0){	
		//prints feedback to user that terminal value was given
		fputs("Quit Value Given \n", stdout);
		//close(client_fd);//closes port with client
		close(fd);//closes listening port
		exit(0);//terminates program
	
	}
	//Print out received message	
	if(strcmp(buffer, "x\n\0")!=0){
		fprintf(stdout, "Received Message: %s", buffer);
	}
	
	}
	//For error recv message
	if(n<0){
		//Generate Error
		//Send error to client	
		send(fd,strerror(errno), BUFSIZE, 0); 	
		//Print error
		fprintf(stderr, "speak.c error: %s\n", strerror(errno));
	}
	//Return to main function with an error
	return -1;
}



void speak( int server_number, char *server_node )
{
int			length;
int			n, len;
ssize_t			ackLen;
short			fd;
struct sockaddr_in	address;
struct hostent		*node_ptr;
char			local_node[NAMESIZE];
char			buffer[BUFSIZE];
char			buffer2[BUFSIZE];



//ASCII Header
printf("███████ ██████  ███████  █████  ██   ██\n");
printf("██      ██   ██ ██      ██   ██ ██  ██\n");
printf("███████ ██████  █████   ███████ █████\n");
printf("     ██ ██      ██      ██   ██ ██  ██\n");
printf("███████ ██      ███████ ██   ██ ██   ██\n");
printf("------------------------------------------------------------\n");
printf("------------------------------------------------------------\n");
/*  get the internet name of the local host node on which we are running  */
if( gethostname( local_node, NAMESIZE ) < 0 )
	{
	perror( "speak gethostname" );
	exit(1);
	}
fprintf(stderr, "speak running on node %s\n", local_node);

/*  get the name of the remote host node on which we hope to find server  */
if( server_node == NULL )
	server_node = local_node;
fprintf(stderr, "speak about to connect to server at port number %d on node %s\n",
		server_number, server_node);

/*  get structure for remote host node on which server resides  */
if( (node_ptr = gethostbyname( server_node )) == NULL )
	{
	perror( "speak gethostbyname" );
	exit(1);
	}

/*  set up Internet address structure for the server  */
memset(&address, 0, sizeof(address));
address.sin_family = AF_INET;
memcpy(&address.sin_addr, node_ptr->h_addr, node_ptr->h_length);
address.sin_port = htons(server_number);

fprintf(stderr, "speak full name of server node %s, internet address %s\n",
		node_ptr->h_name, inet_ntoa(address.sin_addr));

/*  open an Internet tcp socket  */
if( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
	perror( "speak socket" );
	exit(1);
	}

/*  connect this socket to the server's Internet address  */
if( connect( fd, (struct sockaddr *)&address, sizeof(address) ) < 0 )
	{
	perror( "speak connect" );
	exit(1);
	}

/*  now find out what local port number was assigned to this client  */
len = sizeof(address);
if( getsockname( fd, (struct sockaddr *)&address, &length ) < 0 )
	{
	perror( "speak getsockname" );
	exit(1);
	}

/*  we are now successfully connected to a remote server  */
fprintf(stderr, "speak at internet address %s, port %d\n",
		inet_ntoa(address.sin_addr), ntohs(address.sin_port));

printf("------------------------------------------------------------\n");
//Initial User Prompt
fputs("Enter 'x' to switch modes, 'xx' to quit\n", stdout);
fputs("Enter message to send: ", stdout);
/*  transmit data from standard input to server  */
while( fgets( buffer, BUFSIZE, stdin ) != NULL )
	{
	len = strlen(buffer);
	
	
	//sending the message	
	if( (n = send(fd, buffer, len, 0)) != len )
		if( n < 0 )
			{
			perror( "speak send" );
			exit(1);
			}
		else
			{
			fprintf(stderr, "speak sent %d bytes, attempted %d\n", n, len);
			break;
			}

	//After send modifications go here
	//Receiving Acknowledgement	
	ackLen = recv(fd, buffer2, BUFSIZE, 0);
	buffer2[ackLen] = '\0';
	if(ackLen > 0){
		//Acknowledgemnet Received from listener
		if(strcmp(buffer2, "Message Received") == 0){
			//Adding spaces before message for readability
			printf("     Acknowledgement Received\n");	
			
		}
		//Received Error from Listener
		else{	
			printf("speakd.c error: %s\n", buffer2);	
			close(fd);//closes port
			exit(0);//terminates program	
		}
	}
	else{
		//Error receiving anything
		fprintf(stderr, "speakd.c %s\n", strerror(errno));
		close(fd);//closes port
		exit(0);//terminates program
	}
	//If entered a non-special character,
	//Prompt user for more input
	if(strcmp(buffer,"x\n\0")!=0&&strcmp(buffer, "xx\n\0"))
	{
		fputs("Enter message to send: ", stdout);
	}
	//If we sent out x
	if(strcmp(buffer, "x\n\0") == 0){
		if(receiveMode(fd)<0){
			close(fd);
			exit(0);
		}
		//User prompt
		fputs("**********Returning to Send Mode**********\n", stdout);
		fputs("Enter 'x' to switch modes, 'xx' to quit\n", stdout);
		fputs("Enter a message to send: ", stdout);
	}
	
	//If we sent out xx
	if(strcmp(buffer, "xx\n\0") == 0){
		//Prints feedback
		fputs("Quit Value Given \n", stdout);
		close(fd);//closes port
		exit(0);//terminates program

	}


	}

/*  close the connection to the server  */
if( close(fd) < 0 )
	{
	perror( "speak close" );
	exit(1);
	}
}
