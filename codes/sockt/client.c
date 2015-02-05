
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>

#define MY_PORT 12346

main(int argc, char *argv[]) 
{
  int sock;   // socket indentifier
  int br;     // error code for bind
  int fd;     // file handle for open socket

  int cl;     // length of returned client sockaddr_in
  int sl;     // length of server sockaddr_in

  char input_buf[1024];  // character string for input buffer
  int n;                 // length of input buffer
  char message[1024];    // character string for output buffer
  int ml;                // length of output buffer
  
  struct sockaddr_in server;  // socket data types for outgoing 
                                      // and incoming sockets
  struct hostent *theserver;
  char host[1024];



// create a socket
  sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock <0) {
    printf(" socket creation fails \n");
    exit(1);
  }

  // set up the server name
  strcpy(host, "localhost");
  theserver = (struct hostent *) gethostbyname(host);
  if (theserver == NULL) {
    printf("no such host \n");
    exit(1);
  }

  // initialize the server attributes 
  // clean out the memory
  bzero((char *) &server, sizeof(server));
  // set the connection type for internet
  server.sin_family = AF_INET;
  // copy the host information from above into the data structure
  bcopy((void *)theserver->h_addr, 
	(void *)&server.sin_addr,
	theserver->h_length);
  // set the port
  server.sin_port = htons(MY_PORT);
  sl = sizeof(server);
  

  // connect to the socket
  ml = connect(sock, (struct sockaddr *) &server, sl);
  if (ml < 0) {
    printf("error in connect \n");
    exit(1);
  }

  
  // write an output message to the server
  strcpy(message, "now is the time for all good men to come to the aid of their party\n");
  n = write(sock, message, strlen(message));

  // read the returning message
  input_buf[0] = '\0';
  n = read(sock, input_buf, 1024);
  printf("read %s from server \n",input_buf);

}

