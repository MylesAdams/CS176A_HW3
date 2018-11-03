#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#define BUFFERSIZE 1024

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    perror("Invalid number of arguments");
    exit(EXIT_FAILURE);
  }

  int Sockfd;
  char OutBuffer[BUFFERSIZE];
  char InBuffer[BUFFERSIZE];
  struct sockaddr_in ServAddr;

  int Port = strtol(argv[2], (char **)NULL, 10);

  if ((Sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&ServAddr, 0, sizeof(ServAddr));

  ServAddr.sin_family = AF_INET;
  ServAddr.sin_port = htons(Port);
  ServAddr.sin_addr.s_addr = inet_addr(argv[1]);

  srand(time(NULL));
  int i;

  for (i = 0; i < 10; ++i)
  {
    memset(OutBuffer, 0, BUFFERSIZE);
    memset(InBuffer, 0, BUFFERSIZE);

    sprintf(OutBuffer, "PING %d %d", i+1, );

    sendto(
        Sockfd,
        (const char *)OutBuffer,
        strlen(OutBuffer) + 1,
        0,
        (const struct sockaddr *) &ServAddr,
        sizeof(ServAddr));
  }



}
