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

  epfd = epoll_create1(0);
  if (epfd == -1) {
    printf("error:epoll_create");
    return -1;
  }

  /* Open each file on command line, and add it to the "interest
     list" for the epoll instance */

  int sv[2]; // file descriptor pair
  if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sv) == -1) {
    perror("socketpair");
    exit(EXIT_FAILURE);
  }

  int fd = sv[0];
  // Check if if there is input on fd
  ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP ;            
  ev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }

  //close(sv[1]);
  shutdown(sv[1], SHUT_WR);

  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait");
    return -1;
  }

  printf("Ready: %d\n", ready);

  /* Deal with returned list of events */

  for (int j = 0; j < ready; j++) {

    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN ready \n");
    }            

    // Do something if output is possible on the event. 
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT triggered \n");

    }            

    if (evlist[j].events & EPOLLHUP) {
      printf("EPOLLHUP triggered \n");
      }
    if (evlist[j].events & EPOLLRDHUP) {
      printf("EPOLLRDHUP triggered");
      }
  }
  return 0;
}
