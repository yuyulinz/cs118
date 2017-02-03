/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/

//IMPORTANT NOTE: this code was written using the example code provided on CCLE


#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <string.h> //memset
#include <unistd.h> //close
#include <sys/wait.h>	/* for the waitpid() system call */
#include <signal.h>	/* signal name macros, and the kill() prototype */

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void fileHandler (int socksthecat)
{
    int msg;
    char buffer[512];
    char fileName[512];
    
    memset(buffer, 0, 512);
    memset(fileName, 0, 512);
    
    msg = read(socksthecat, buffer, 255);
    if (msg < 0)
        error("ERROR reading from socket\n");
    printf("Here is the buffered message: %s\n", buffer);
    
    
    
    
    return;
}



int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen; //int of length at least 32-bits
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);	//create socket
     if (sockfd < 0) 
        error("ERROR opening socket");
     memset((char *) &serv_addr, 0, sizeof(serv_addr));	//reset memory
     //fill in address info
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY; //sin_addr has one paramter, unsigned long. INADDR_ANY is server ip address
     serv_addr.sin_port = htons(portno); //convert portno to network byte order, big endian??
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, //serv_addr must be cast to sockaddr pointer
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd,5);	//5 simultaneous connection at most
    
    clilen = sizeof(cli_addr);
    
    //need to setup a persistent server
    while (1)
    {
        //accept connections
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
        if (newsockfd < 0)
            error("ERROR on accept");
        
        pid = fork(); //create new process to handle HTTP requests
        if (pid < 0)
            error("Error on fork");
        
        else if (pid == 0)
        {
            close(sockfd);
            fileHandler(newsockfd);
            exit(0); //successful file transfer or request
        }
        
        else
            close(newsockfd);
    }
    return -1; //we return -1 bc if this is executed, something went horribly wrong
}

