#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/eventfd.h>
#include <sys/socket.h>

// This is to test the case of two epfd
int main(int argc, char *argv[]) {

  int a = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP | EPOLLET;
  printf("%d", a);
  return 0;
}
