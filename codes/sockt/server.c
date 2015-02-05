
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
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
  
  struct sockaddr_in server, client;  // socket data types for outgoing 
                                      // and incoming sockets
  
  // create a socket
  sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock <0) {
    printf(" socket creation fails \n");
    exit(1);
  }
  
  // set up the values in the data structure to receive messages
  // from any port over the internet
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(MY_PORT);
  sl = sizeof(server);
  
  // bind the server parameters to the socket
  br = bind(sock, (struct sockaddr *) &server, sl);
  if (br < 0) {
    printf(" binding failed \n");
    exit(1);
  }
  
  // listen for a message
  listen(sock, 5);

  // verify that the system is listening on the right port
  getsockname(sock, (struct sockaddr *) &server, &sl);
  printf("listening on port %d \n", ntohs(server.sin_port));
  
  // enter an infinite loop to wait for a message
  while(1) {
    

    // accept an incoming communication
    fd = accept(sock, 
		(struct sockaddr *) &client, &cl);    
    if (fd < 0) {
      printf ("error in accepting client \n");
      exit(1);
    }
    

    // read the data being sent from the client and print it out locally
    n = read(fd, input_buf, 1024);
    printf("%s\n",input_buf);

    // load a message into the output buffer, and then write it
    strcpy(message,"our finest hour \n");
    ml = strlen(message);
    write(fd, message, ml);

    // close the socket - hang up on the client
    close(fd);
    
  }

}
