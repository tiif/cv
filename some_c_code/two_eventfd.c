#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/eventfd.h>
#include <sys/socket.h>

#define MAX_BUF     1000        /* Maximum bytes fetched by a single read() */
#define MAX_EVENTS     5        /* Maximum number of events to be returned from
                                   a single epoll_wait() call */

// This is to test the case of two epfd
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

  // create an eventfd instance. 
  int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  printf("efd is %d \n", efd);
  printf("opened a socketpair \n");

  uint64_t val = 18446744073709551614;

  // Write to eventf
  int res = write(efd,&val, sizeof(uint64_t));
  printf("write to a eventfd\n");
  printf("result of write is %d \n", res);


  val = 1;

  // Write to eventf
  res = write(efd,&val, sizeof(uint64_t));
  printf("write to a eventfd\n");
  printf("result of write is %d \n", res);

  int fd = efd; 
  // Check if if output is possible on fd
  ev.events = EPOLLIN | EPOLLOUT | EPOLLET;            
  ev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }


  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait");
    return -1;
  }
  printf("Ready: %d\n", ready);


  for (int j = 0; j < ready; j++) {
    if (evlist[j].events & EPOLLET) {
      printf("EPOLLET \n");
    }

    // Do something if output is possible on the event. 
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT \n");
    }
    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN \n");
    }            
    if (evlist[j].events & EPOLLERR) {
      printf("EPOLLERR \n");
    }            
  }

  // Read from eventfd
  uint64_t  u;
  res = read(efd, &u, sizeof(uint64_t));
  printf("result for read is %d \n", res);



  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait");
    return -1;
  }
  printf("Ready: %d\n", ready);


  for (int j = 0; j < ready; j++) {
    if (evlist[j].events & EPOLLET) {
      printf("EPOLLET \n");
    }

    // Do something if output is possible on the event. 
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT \n");
    }
    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN \n");
    }            
  }

  return 0;
}
