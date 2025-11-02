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

  int fd = sv[0];
  // Check if if there is input on fd
  // EPOLLIN check if there is input for the fd 
  ev.events = EPOLLIN | EPOLLET;            
  ev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }

  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }
  // Do both input and output. 
  write(sv[1], "a", 1);
  printf("write to socketpair \n");
  read(sv[1], "a", 1); 
  printf("read from socketpair \n");
  write(sv[1], "a", 1);
  printf("write to socketpair \n");


  // First epoll_wait. 
  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }

  printf("Ready: %d\n", ready);

  // Second epoll_ctl to track output
  ev.events = EPOLLOUT | EPOLLET;             
  ev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }

  // Second epoll_wait. 
  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }

  printf("Ready: %d\n", ready);
  /* Deal with returned list of events */

  for (int j = 0; j < ready; j++) {

    // If output happened to the fd. 
    if (evlist[j].events & EPOLLOUT) {
      printf("output happened \n");
    }

    // If input happened to the fd. 
    if (evlist[j].events & EPOLLIN) {
      printf("input happened \n");
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

//Note: 
//1. EPOLLIN and EPOLLOUT both happen and be observed
//2. If both EPOLLIN and EPOLLOUT is set, one happen then it will be returned, and the
//   flag is not changed at all. 
//   1. track input
//   2. do input 
//   3. do output
//   4. epoll_wait
//   5. epoll_ctl to track output 
//   6. epoll_wait (since the output is not tracked last time, will it be tracked?)
