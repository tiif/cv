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
  printf("sv[0] is %d \n", sv[0]);
  printf("sv[1] is %d \n", sv[1]);

  // Set minimal buffer size
  int sendbuff = 1;
  printf("sets the send buffer to %d\n", sendbuff);
  int res = setsockopt(sv[1], SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff));
  printf("res of setsockopt is %d \n", res);
  printf("sets the receive buffer to %d\n", sendbuff);
  res = setsockopt(sv[0], SOL_SOCKET, SO_RCVBUF, &sendbuff, sizeof(sendbuff));
  printf("res of setsockopt is %d \n", res);

  // Register both side with epoll_ctl
  int fd = sv[0];
  ev.events = EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLET;            
  ev.data.fd = fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }
  fd = sv[1];
  ev.events = EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLET;            
  ev.data.fd = fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }

  // Try to write until it is full
  int counter = 0;
  res = 0;
  while (res != -1) {
    res = write(sv[1], "abcde", 5);
  }
  if (errno == EAGAIN) {
    printf("Buffer is full \n");
  }

  // epoll_wait  
  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }
  printf("Ready: %d\n", ready);

  // Parse and print the ready event. 
  for (int j = 0; j < ready; j++) {
    printf("this is the event %d \n", evlist[j].data.fd);
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


  // Read from the other side. 
  res = read(sv[0], &buf0, 1);
  printf("result of read is %d \n", res);

  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }
  printf("Ready: %d\n", ready);

  // Parse and print the ready event. 
  for (int j = 0; j < ready; j++) {
    printf("this is the event %d \n", evlist[j].data.fd);
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
  return 0;
}

