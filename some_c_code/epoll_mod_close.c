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

// If attempt to add, delete, modify a file description that is fully closed, fail with ``EBADF``
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
  printf("opened a socketpair \n");
  write(sv[1], "a", 1);
  printf("write to socketpair \n");

  int fd = sv[0];
  // Check if if output is possible on fd
  ev.events = EPOLLIN | EPOLLET;            
  ev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }

  // Close the fd used to register the event. 
  close(fd);
  fd = sv[1];
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }
  /* Fetch up to MAX_EVENTS items from the ready list */

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
      int s = read(evlist[j].data.fd, buf, MAX_BUF);
      if (s == -1) {
        printf("read \n");
        return -1;
      }
      printf("read %d bytes: %.*s\n", s, s, buf);

    }            
  }
  return 0;
}
