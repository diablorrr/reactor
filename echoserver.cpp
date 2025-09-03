#include <cstdio>
#include <signal.h>
#include "EchoServer.h"

EchoServer* echoserver;

void Stop(int sig) {
  printf("sig=%d\n", sig);

  echoserver->Stop();
  printf("echoserver已停止。\n");
  delete echoserver;
  printf("delete echoserver。\n");
  exit(0);
}


int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("usage: ./echoserver ip port\n");
    printf("example: ./echoserver 192.168.150.128 5085\n\n");
    return -1;
  }

  signal(SIGTERM, Stop);
  signal(SIGINT, Stop);

  echoserver = new EchoServer(argv[1], atoi(argv[2]), 3, 2);
  echoserver->Start();

  return 0;
}
