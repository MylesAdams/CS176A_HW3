#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <float.h>

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
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  struct timeval tval;
  tval.tv_sec = 1;
  tval.tv_usec = 0;

  if (setsockopt(Sockfd, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)) < 0)
  {
    perror("Failed to set socket timeout");
    exit(EXIT_FAILURE);
  }

  memset(&ServAddr, 0, sizeof(ServAddr));

  ServAddr.sin_family = AF_INET;
  ServAddr.sin_port = htons(Port);
  ServAddr.sin_addr.s_addr = inet_addr(argv[1]);

  srand(time(NULL));

  struct timespec InitialTimeSpec, FinalTimeSpec;
  long InitialUS, FinalUS;
  time_t InitialS, FinalS;
  long InitialTime, FinalTime;
  double TimeDiff;
  socklen_t len;

  int PacketsReceived = 0;
  double MaxRTT = 0.0;
  double MinRTT = DBL_MAX;
  double AvgRTT = 0.0;

  int i;
  for (i = 0; i < 10; ++i)
  {
    memset(OutBuffer, 0, BUFFERSIZE);
    memset(InBuffer, 0, BUFFERSIZE);

    // Populate initial timepsec
    clock_gettime(CLOCK_REALTIME, &InitialTimeSpec);

    // Calculate initial time from timespec in microseconds
    InitialS = InitialTimeSpec.tv_sec;
    InitialUS = InitialTimeSpec.tv_nsec / 1000;
    InitialTime = (InitialS * 1000000) + (InitialUS);

    snprintf(OutBuffer, BUFFERSIZE, "PING %d %ld%ld", i+1, (long)InitialS, InitialUS);

    // Send PING to server
    sendto(
           Sockfd,
           (const char *)OutBuffer,
           strlen(OutBuffer) + 1,
           0,
           (const struct sockaddr *) &ServAddr,
           sizeof(ServAddr));

    // Recv from server, only calculate RTT statistics if successful
    if (recvfrom(
             Sockfd,
             (char *)InBuffer,
             BUFFERSIZE,
             MSG_WAITALL,
             (struct sockaddr *)& ServAddr,
             &len) > 0)
    {
      // Populate final timespec
      clock_gettime(CLOCK_REALTIME, &FinalTimeSpec);

      // Calculate final time from timespec in microseconds
      FinalS = FinalTimeSpec.tv_sec;
      FinalUS = FinalTimeSpec.tv_nsec / 1000;
      FinalTime = (FinalS * 1000000) + (FinalUS);

      // Get time difference
      TimeDiff = (FinalTime - InitialTime) / 1000.0;

      // If incoming message matches prior outgoing message
      // Increase number of packets received and calculate stats regarding RTT
      if (!strncmp(InBuffer, OutBuffer, BUFFERSIZE))
      {
        PacketsReceived++;

        AvgRTT += TimeDiff;

        if (TimeDiff > MaxRTT)
        {
          MaxRTT = TimeDiff;
        }

        if (TimeDiff < MinRTT)
        {
          MinRTT = TimeDiff;
        }

        printf("PING received from %s: seq#=%d time=%.3lf ms\n", argv[1], i + 1, TimeDiff);

      }
      // Sleep the rest of the second, so that each PING is sent about 1 second
      // apart, like the assignment said.
      usleep((useconds_t)(1000000 - (TimeDiff * 1000)));
    }

  }

  AvgRTT /= PacketsReceived;

  // Print out statistics
  printf("--- ping statistics --- 10 packets transmitted, %d packets received, %.2f%% packet loss rtt min/avg/max = %.3lf %.3lf %.3lf ms\n", PacketsReceived, ((10 - PacketsReceived) / 10.0) * 100, MinRTT, AvgRTT, MaxRTT);


}
