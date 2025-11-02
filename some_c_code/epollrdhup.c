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


  // Register the file descriptor with epoll_ctl. 
  int fd = sv[1];
  // Check if if there is input on fd
  // EPOLLIN check if there is input for the fd 
  ev.events = EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLET;            
  ev.data.fd = fd;


  // Write to the socketpair
  printf("write here \n");
  int res = write(sv[0], "abcde", 5);
  printf("%d \n", res);


  // close the peer. 
  close(sv[0]);

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("err: epoll_ctl\n");
    printf("errno is %d \n", errno);
    return -1;
  }

  // epoll wait the instance. 
  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }
  printf("Ready: %d\n", ready);

  // Parse and print the ready event. 
  for (int j = 0; j < ready; j++) {
    // If the peer close the connection. 
    if (evlist[j].events & EPOLLRDHUP) {
      printf("epollrdhup happened \n");
    }

    // If output happened to the fd. 
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT happened \n");
    }

    // If input happened to the fd. 
    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN triggered\n");
    }            

  }

  // Read from the other side. 
	res = read(sv[1], &buf0, 1);
  printf("result of res is %d\n", res);

  // epoll wait the instance. 
  printf("About to epoll_wait \n");
  ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
  if (ready == -1) {
    printf("epoll_wait return -1");
    return -1;
  }
  printf("Ready: %d\n", ready);

  // Parse and print the ready event. 
  for (int j = 0; j < ready; j++) {
    // If the peer close the connection. 
    if (evlist[j].events & EPOLLRDHUP) {
      printf("epollrdhup happened \n");
    }

    // If output happened to the fd. 
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT happened \n");
    }

    // If input happened to the fd. 
    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN triggered\n");
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
