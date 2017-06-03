/**
* 	client.c    - This file creates the client (presenter or listener) for the whiteboard console program
*
*   Description - The client.c file calls connect to make a new connection and the user enters 
*   			  username and password. After authentication an appropriate function is called according to
*  				  that for presenter or listener.
* 
* 	Submission 	- This file is submitted as Project Assignment: 'CS-603 Network Programming' ' to respected
*   Details       Sir Karan Gupta, S.G.T.B Khalsa College by Manpreet Kaur (Roll No. 1217) & Kushagra Gupta 
* 				  (Roll No. 1211) (6th Semester)
* 	Submission  - 16th April, 2016.       
* 	Date 		 
*       
**/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include "conio.h"

#define MAXLINE 4096

char recvline[MAXLINE];

/** Function to be called in case of any erroneous situation 
 *  Input : A string message									 
 *  Output: The message displayed on the screen and process terminated
**/
void err_sys(const char* x) 
{ 
	/* perror() prints discriptive error message to stderr*/
    perror(x); 
    exit(1); 
}  



/** Function to be called for the presenter client
 *  Input : The file descriptor to take input from & sockfd of the presenter client									 
 *  Output: Takes input from user char-by-char and sends it to server until user press ^C
**/
void cli_presenter(FILE *fp, int sockfd)
{	
	/* setting the title of the window  to 'Presenter' */
	system("wmctrl -r :ACTIVE: -N \"Presenter\"");
	
	/* we are sending character by character & fgets require buffer */
	char sendline[1]; 
	
	/* Reading the character & writing the read character to the server */
	while(sendline[0] = getche())
		if( write(sockfd, sendline, 1) < 0 )
			err_sys("Error Writing the character by presenter");
}


/** Function to be called for the listener client
 *  Input : The sockfd of the listener client									 
 *  Output: It displays the received data from the server until the presenter leaves
**/
void cli_listener(int sockfd)
{
	/* setting the title of the window to 'Listener' */
	system("wmctrl -r :ACTIVE: -N \"Listener\"");
	
	char recvline[4096];
	fflush(stdout); 
	int n;
	
	while(1)
	{   
		/* Reading the data sent by the server and printing it to terminal */
		if((n = read(sockfd, recvline, sizeof(recvline))) > 0)	
		{   
			/* When the presenter leaves, '!' is written to the file */
			if(recvline[0] == '!')
			{ 
				fputs("\nPresentation is over, thank you for listening!!!\n",stdout);
				fflush(stdout);
				exit(0);
		    }
			fputs(recvline, stdout);
		    fflush(stdout); 
		
			memset(recvline,0,4096);}
			else
			  err_sys("cli_listener : server terminated prematurely");
	}
}
		
		
		
/** Main   : Entry point of the program
 *  Purpose: Makes the socket descriptors and establishes connection with the server
 *  	     by taking u-name & password for authentication
**/
int main( int argc, char **argv)
{  
	/* Clearing the terminal screen (system-call) */
	system("clear");
	
	int sockfd;
	struct sockaddr_in servaddr;
	
	/* Buffer for u-name & password */
	char username[50];  
	char password[50];  
	
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	
	/* If the IP of sever is not provided in input */
	if (argc != 2)
	  err_sys("usage: tcpcli <127.0.0.1>");
	  
	/* Creating a socket for the client of SOCK-STREAM type */  
	sockfd = socket (AF_INET, SOCK_STREAM,0);
	if(sockfd < 0)
		err_sys("Error in creating the socket");
	
	/* Clearing the socket address structure */
	bzero(&servaddr , sizeof(servaddr));
	
	/* Storing the information in the socket address structrure of client */
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (10000);
	inet_pton ( AF_INET, argv[1], &servaddr.sin_addr );
	
	
	/* Calling the server to connect (active-connect) */
	if(connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
		printf("Connect Error");
	
	char charUser; 
	char charPass; 
	int i = 0;
	
	/* Taking the username input of client from console */
	printf("Enter Username :");
	fflush(stdout);
	do{
	    charUser = getche();
		username[i] = (char) charUser;
		i++;
	}while(charUser != '\n');
	
	fflush(stdin);
	i = 0;
	
	/* Taking the password input of client from console */
	printf("\nEnter Password :");
	fflush(stdout);
	do{
	    charPass = getche();
		password[i] = (char) charPass;
		i++;
	}while(charPass != '\n');
	fflush(stdin);
		
	/* Sending both the u-name & password of the client to server */	
	if( write(sockfd, username, sizeof(username)) < 0)
		err_sys("Error in writing username");
	if( write(sockfd, password, sizeof(password)) <0 )
		err_sys("Error in writing password");
	
	/* Reading its status in presentation */	
	if(read(sockfd,recvline,MAXLINE) <= 0)
		  err_sys("str_cli: server terminated prematurely");
	
	/* Clearing the console for presentation */		  
	system("clear");	
		
	fputs(recvline,stdout);
	fflush(stdout);
	fputs("\n\n", stdout);
	fflush(stdout);
	
	/* If the client is Prsenter, call the corresponding writing fucntion */	
	if(strcmp(recvline,"Presenter") == 0 )
	{
		system("notify-send \"Connection Established\"");
		cli_presenter(stdin, sockfd);
	}
	
	/* If the client is Listerner, call the corresponding writing fucntion */	
	else if(strcmp(recvline,"Listener") == 0 )
	{
		system("notify-send \"Connection Established\"");
		cli_listener(sockfd);
	}
	
	/* If the client is neither Presenter nor Listener, exit */	
	exit(0);
}
