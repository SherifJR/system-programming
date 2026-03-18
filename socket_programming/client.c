#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<unistd.h>
#include<strings.h>
#include<string.h>

/*
  struct hostent{ // defined in netdb.h
    char* hostname; ==> official name of the host
    char** h_aliasses; --> a list of alternative names for thehost
    int h_addrtype; ==> type of address being returned, till now we use AF_INET
    int h_length; --> the length of the address "in bytes"
    char** h_addr_list; ==> pointer to a list of network addresses for the named host, 
                            -host addresses are returned in network byte order --> why >>>>>>>>
                            -To maintain interoperability, data sent over the network must be converted to network                            byte order (big-endian)"MSB is stored at the lowest address" before transmission and                             converted back to the host’s native byte order upon receipt.
    #define h_addr h_addr_list[0];
  }

  this struct defines a host computer on the internet
*/

void error(char* msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
  int sock_fd, port_no, n;
  struct sockaddr_in serv_addr; //serv_addr will contain the address of the server we want to connect to.
  struct hostent* server;
  char buffer[256];

  if(argc < 3)
  {
    fprintf(stderr, "usage %s hostname port", argv[0]);
    exit(0);
  }

  port_no = atoi(argv[2]);
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  if(sock_fd < 0)
    error("ERROR openning socket \n");
  server = gethostbyname(argv[1]); //argv[1] --> contains the name of the host on the internet."e.g fci.zu.edu"
  if(server == NULL)
  {
    fprintf(stderr, "Error, no such host\n");
    exit(0);
  }

  //struct hostent *gethostbyname(char* name);
  
  bzero((char*) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  
  bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length); //bcopy(char* s1, char* s2, int length) --> copy the length bytes from s1 to s2.
  serv_addr.sin_port = htons(port_no);
  
  if(connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on connecting \n");

  printf("Plz enter your message: ");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);
  n = write(sock_fd, buffer, strlen(buffer));
  if(n < 0)
    error("ERROR on writing to socket \n");
  bzero(buffer, 256);
  n = read(sock_fd, buffer, 255);
  if(n < 0)
    error("ERROR read from socket");
  printf("%s \n", buffer);

  return 0;
}
