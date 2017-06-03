/**
 * 	server.c    - This file creates the server for the  whiteboard console program
 *
 * 	Description - The server.c file creates a listening socket to listen for incoming
 * 				  connections be it presenter or listener. After authentication the 
 * 				  server spawns a child process to handle the respective client and 	 
 * 				  calls a corresponding function required for that of a presenter or 
 * 				  a listener.
 * 
 * 	Submission 	- This file is submitted as Project Assignment : 'CS-603 Network Programming' 
 *  Details       to respected Sir Karan Gupta, S.G.T.B Khalsa College by Manpreet Kaur (Roll No. 1217)
 * 				  & Kushagra Gupta (Roll No. 1211) (6th Semester).
 *	Submission 
 * 	Date 		- 16th April, 2016.	       
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
#include <errno.h>

#define MAXLINE 4096

int count = 0; /* The number of listerners connected*/
char buff[1];  /* The buffer reading the data from presenter*/
FILE *f;	   /* The file in which presentation is stored at server */



/** Function to be called in case of any erroneous situation 
 *  Input : A sting message									 
 *  Output: The message displayed on the screen and process terminated
**/
void err_sys(const char* x) 
{ 
	/* perror() prints discriptive error message to stderr*/
    perror(x); 
    exit(1); 
}



/** Function counts the number of character printed to file when listener arrivers 
 * 	Input : File pointer to a file
 *  Output: The count of number of characters in file
**/
int getFileCount(FILE *fp)
{   
	int char_read = 0;
	
	/* increment the count until EOF is reached */
	while(fgetc(fp) != EOF)
	 char_read++;
	 
	return char_read;	
}



/** Function that interacts with the presenter reading a character & printing to file 
 *  Input : Connection function descriptor of the presenter
 *  Output: The character printed to the file (file.txt)
**/
void serv_handler(int connfd)
{ 
	 /* number of character read in read() */
	 ssize_t n; 
	 
	  /* opening the storing file in rw mode */
     f = fopen("file.txt","w+");
     if(f == NULL)
		err_sys("Error Opening the text file in rw mode");
     
     /* Closing the file so that earlier data is removed */
     fclose(f);	 
     
	 /* reading the character from presenter and storing in file*/
	 again:
	 while(( n = read(connfd, buff, 1) > 0))
	 { 
		 /* Opening the file in appending mode */
		f = fopen("file.txt","a+");
		if(f == NULL)
			err_sys("Error Opening the text file in appending mode");
		fputs(buff,stdout);
		
		/* In an infinite loop to print anything we need to flush it */
		fflush(stdout);
		
		 /* Saving the character in file */
		fputc(buff[0],f);
		
		fclose(f);
     } 
		
     /* Any interrupt occur when read is in a blocking call */	
	if( n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo : read error");	
	else if( n == 0 )
	   {     
		 /* Opening the file in appending mode */
		f = fopen("file.txt","a+");
		if(f == NULL)
			err_sys("Error Opening the text file in appending mode");
		fputs(buff,stdout);
		
		/* In an infinite loop to print anything we need to flush it */
		fflush(stdout);
		
		 /* Saving the character in file */
		fputc('!',f);
		fclose(f);
	    err_sys("\nPresenter left the presentation\n");	
	    fflush(stdout);
	  }
}	



/** Function that interacts with the listener's reading a character from file and sending to client
 *  Input : Connection function descriptor of the listener
 *  Output: The character written to the clients connfd **/
void serv_write	( int connfd)
{   
	int c, n; 
	char send_buff[4096]; 
	int char_read; 		  
	
	 /* Opening the presentation file in 'reading' mode */
	f = fopen("file.txt","r");
	if(f == NULL)
	  err_sys("file not open");
	 
	 /* Getting the character count*/ 
	char_read = getFileCount(f);
	
	/* Returning the file pointer to the beginning */
	rewind(f);					 

	/* Reading the data that was written previous to the listener arrived */
    while(fgets(send_buff,char_read,f) != NULL)
    {
		if( (n=write(connfd,send_buff,strlen(send_buff)))< 0 )
			err_sys("write error");
		
	memset(send_buff,0,4096); /* Clearing the buffer to remove previous read data */
	}

	/* Now reading the character as it is writtern by presenter */
	while(1)
	{   
		c=fgetc(f);
		
		/* Send to client only if it a valid charcter */
		if(isalnum(c) || isspace(c) || (char)c == '\n' || (char)c == '!') 
		{
			buff[0] = (char)c;
			write(connfd,buff,1); 
		}
		
		/* If anything else written skip it and dont write to client */
		else
		{ 
			buff[0] = '\0';
			sleep(1);
	    }
    }
}



/** Main   : Entry point of the program
 *  Purpose: Makes the socket descriptors and establishes connection with the client
 *  	     after authenticating a valid client. 
**/
int main(int argc, char ** argv)
{   
	 /* Clearing the terminal screen (system-call) */
	system("clear"); 							
	 
	 /* setting the title of the window to 'Server' */
	system("wmctrl -r :ACTIVE: -N \"Server\"");
	
	printf("\n\t\t\t  Server is running...!!!\n");
	printf("\n\t\t\t(Waiting for the Presenter)\n");
	
	/* Files for  password verification opened in 'r' mode */
	FILE *f_presenter, *f_listener;
	
	f_presenter = fopen("Presenter_Password.txt", "r");
	if(f_presenter == NULL)
		err_sys("Error in opening Presenter Password file");
	
	f_listener = fopen("Listener_Password.txt", "r");
	if(f_listener == NULL)
		err_sys("Error in opening Listener Password File");
	
	/* u-name & password read form client*/
	char socket_username[50], socket_pass[50]; 
	
	/* u-name & password read form file for listener & presenter */
	char listener_username[50], listener_pass[50];
	char presenter_username[50], presenter_pass[50];
	
	memset(socket_username, 0, sizeof(socket_username));
	memset(socket_pass, 0, sizeof(socket_pass));
	memset(listener_username, 0, sizeof(listener_username));
	memset(listener_pass, 0, sizeof(listener_pass));
	memset(presenter_username, 0, sizeof(presenter_username));
	memset(presenter_pass, 0, sizeof(presenter_pass));
	
	int listenfd, connfd, present=0, n = 0;
	
	
	/* Socket address structures for both client and server */
	struct sockaddr_in cliaddr,servaddr;
	pid_t childpid;
	socklen_t clilen;
	char send[10];
	
	
	/* Creating a socket for server that will remain in listening mode */
	listenfd = socket(AF_INET, SOCK_STREAM,0);
	if(listenfd < 0)
		err_sys("Error in creating the socket");
	
	/* Storing the infromation in server socket address structure */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(10000);
	
	/* Binding the provided information to the listening socket */
	if( (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0)
		err_sys("Bind Error");
	
	/* The server listening for connections (approx 1.5 * size) */
	if( listen(listenfd, 50) < 0)
		err_sys("Listen Error");
	
	/* reading the presenter file for u-name & password */ 
	fgets(presenter_username, sizeof(presenter_username), f_presenter);
	fgets(presenter_pass, sizeof(presenter_pass), f_presenter);
	/* reading the listener file for u-name & password */ 
	fgets(listener_username, sizeof(listener_username), f_listener);
	fgets(listener_pass, sizeof(listener_pass), f_listener);
	
	/* Handling any incoming client */ 
	for( ; ; )
	{  
		clilen = sizeof(cliaddr);
		
		/* The server is blocked in the call of accept to accept connections */ 
		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen); 
		if(connfd < 0)
			err_sys("Accept Error");
		
		/* default value to be sent to the client side */
		strcpy(send,"fail");
		
		/* Reading the username from the client */
		do{
			n = read(connfd, socket_username, sizeof(socket_username));
			if(n == 0) /* Handling the situation if client exit while typing u-name */
				printf("Guest Left\n");
		}while(n < 0);
			
	    /* Reading the password from the client */
		do{
			n  = read(connfd, socket_pass, sizeof(socket_pass));
			if(n == 0) /* Handling the situation if client exit while typing pass */
				printf("Guest Left\n");
		}while (n < 0);
		
		
		/* Comparing the entered data first with that of presenter */
		if( strcmp(socket_username, presenter_username) == 0 && strcmp(socket_pass, presenter_pass) == 0 )
		{
			fflush(stdout);
			/* If no presentation is already going on */
			if(present == 0)
			{
		     system("clear");
			 present = 1; /* Setting the presentation flag */
				
			 strcpy(send,"Presenter");
			}
			/* If the presentation is already going on flag error */
			else if(present == 1)
			 {
				 write(connfd,"Presentation is already going on. Connect again to join as Listener", strlen("presentation is already going on. Connect again to join as Listener") );
				 close(connfd);
			 }
		}
		
		
		/* Comparing the entered data with that of listener  */
		else if( strcmp(socket_username, listener_username) == 0 && strcmp(socket_pass, listener_pass) == 0 )
		{
			/* If no presentation is going on and listener arriveed : Flag error */
			if(present == 0)
			{
				write(connfd,"No presentation is going on. Try again later!!!", strlen("No presentation is going on. Try again later!!!") );
				close(connfd);
			}
			/* If a presentation is ON add the client as listener */
			else if(present == 1)
			{
				count++; /* Increment the number of listener in the presentation */
				
				strcpy(send,"Listener");
			}
		}
		
		/* If the entered information is incorrect : Flag error */
		else
		{
			write( connfd,"Invalid Username or Password", strlen("Invalid Username or Password") );
			close(connfd);
		} 
		
		
		/* If the entered infomation is valid to the scenario */
		if( strcmp(send, "Presenter") == 0 || strcmp(send, "Listener") == 0 )
		{
			/* Spawning a child process to handle the client */
			childpid = fork();
			
			/* Code to be run in the child process */
			if(childpid == 0)
			{ 
				/* Closing the listening socket of process to avoid multiple listen sockets */	
				close(listenfd);
			
				/* Notification popup on the server side */
				system("notify-send \"New Client Arrived\"");
			
				/* Send the information to the client about its status in the presentation */
				write(connfd,send,sizeof(send));
			
			
				/* If the client is Presenter, call the corresponding reading function */
				if(strcmp(send,"Presenter") == 0)
					serv_handler(connfd);
				
				/* If the client is a Listener, call the corresponding writing function  */
				else if(strcmp(send,"Listener") == 0)
					serv_write(connfd);
			
				/* Closing the child process when the client is handled */
				exit(0);
			}
			
		/* Closing the connection fd of sever to avoid multiple connections to client */	
		
		close(connfd);
		}
	}
}
