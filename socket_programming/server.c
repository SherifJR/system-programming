#include<stdio.h>
#include<sys/types.h>   // for data needed for system calls 
#include<sys/socket.h>  // includes what we need for socket
#include<netinet/in.h> // for internet domain addresses
#include<stdlib.h>
#include<unistd.h>
#include<strings.h>

void error(char* msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}


/* 
struct sockaddr_in{;
  short sin_family; must be AF_INET
  u_short sin_port;
  struct in_addr sin_addr;
  char sin_zero[8]; not used must be 0
}
*/

int main(int argc, char* argv[])
{
  int sock_fd, newsock_fd, portno, cli_len, n; // sock_fd & newsock_fd will store the values returned from socket system call and the accept system call.
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr; // serv_addr / cli_addr:contains address, one of server and another of client.
  
  if(argc < 2)
  {
    fprintf(stderr,"ERROR, no port provided");
    exit(EXIT_FAILURE);
  }
  sock_fd = socket(AF_INET, SOCK_STREAM, 0); // socket syscall takes 3 arguments: 
                                             //the 1st is the address domain of the socket for unix will be AF_UNIX for internet will be AF_INET, and there other possible options if needed.
                                             //the 2nd is the type of the socket. SOCK_STREAM | SOCK_DGRAM.
                                             //the 3rd is the protocol "it should be always 0" --> OS chooses the appropriate protocol.
                                             //
  
  if(sock_fd < 0) //-1 means system call failed
    error("ERROR opening socket");  
  bzero ((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]); // the port number on which the server will listen for to connection, use atoi() to convert this from a string of digits to an integer.
  serv_addr.sin_family = AF_INET; //code for the address family
  serv_addr.sin_port = htons(portno); //htons()==> converts a port number in host byte order to a port number in network byte order which is necessary.
  serv_addr.sin_addr.s_addr = INADDR_ANY; //IP address of the host.
  
  if(bind(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
  //bind takes 3 arg: 1st is socket fd.
  //2nd is the address to which it is bound.
  //3rd is the size of that address we passed.
  
  listen(sock_fd, 5); //the number 5 "backlog queue" is the number of connections can be waiting while the process is handling a particular connection, 5 is the max in most systems. 
  
  cli_len = sizeof(cli_addr);
  newsock_fd = accept(sock_fd, (struct sockaddr*) &cli_addr, &cli_len);//accept syscall cause the system to block until the client connects to the server, it wakes up the process when a connection from a client has been         successfully established. It returns a new file descriptor.
  //2nd arg is reference pointer to the address of the client on the other end of connection.
  //3rd is the size of this structure.
  if(newsock_fd < 0)
    error("ERROR on accept");

  bzero(buffer, 255);
  n = read(newsock_fd, buffer, 255);
  if(n<0)
    error("ERROR reading from the socket");
  printf("Here is the message: %s \n", buffer);

  return 0;
}
