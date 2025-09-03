#include <cstdio>
#include <signal.h>
#include "EchoServer.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("usage: ./echoserver ip port\n");
    printf("example: ./echoserver 192.168.150.128 5085\n\n");
    return -1;
  }
}
