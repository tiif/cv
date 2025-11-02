#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {

  // Just an example of file description 
  int sv[2];   
  if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sv) == -1) {
    perror("socketpair");
    exit(EXIT_FAILURE);
  }

  // fd2 point to sv[0] file description too. 
  int fd2 = dup(sv[0]);
  printf("fd2 is: %d\n", fd2);
  // call dup2 again.  
  int res = dup2(sv[0], fd2);
  // It is expected to be successful.
  printf("result is: %d \n", res);
}

