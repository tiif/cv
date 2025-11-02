#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>

#define MAX_BUF     1000        /* Maximum bytes fetched by a single read() */
#define MAX_EVENTS     5        /* Maximum number of events to be returned from
                                   a single epoll_wait() call */

int main(int argc, char *argv[]) {

  int epfd;
  struct epoll_event ev;
  struct epoll_event evlist[MAX_EVENTS];
  int ready;
  char buf[MAX_BUF];
  char buf0, buf1;

  // Create an epoll instance.
  epfd = epoll_create1(0);
  if (epfd == -1) {
    printf("error:epoll_create");
    return -1;
  }

  // Open socketpair instance.
  int sv[2]; // file descriptor pair
  if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sv) == -1) {
    perror("socketpair");
    exit(EXIT_FAILURE);
  }
  printf("opened a socketpair \n");


  // Register sv[1] to epoll .
  int fd = sv[1];
  ev.events = EPOLLOUT | EPOLLIN | EPOLLET;
  ev.data.fd = fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }

  // Write to sv[0]
  write(sv[0], "c", 1);

  // Invoke epoll_wait.
  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }
  printf("Ready: %d\n", ready);

  // Parse and print the ready event.
  for (int j = 0; j < ready; j++) {
    printf("this is the event for fd: %d \n", evlist[j].data.fd);
    if (evlist[j].events & EPOLLRDHUP) {
      printf("EPOLLRDHUP happened \n");
    }
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT happened \n");
    }
    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN happened\n");
    }
  }
}
