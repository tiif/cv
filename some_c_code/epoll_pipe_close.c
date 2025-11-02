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

  // Open pipe instance. 
  int sv[2]; // file descriptor pair
  if (pipe(sv) == -1) {
    perror("pipe creation failed");
    exit(EXIT_FAILURE);
  }
  printf("opened a pipe\n");

  // Set nonblocking read
  if (fcntl(sv[0], F_SETFL, O_NONBLOCK) == -1) {
    perror("pipe non block failed");
    exit(EXIT_FAILURE);
  }

  if (fcntl(sv[1], F_SETFL, O_NONBLOCK) == -1) {
    perror("pipe non block failed");
    exit(EXIT_FAILURE);
  }

  //int res = write(sv[1], "c", 1);
  //printf("result of res is %d\n", res);
  close(sv[1]);

  // Register the file descriptor with epoll_ctl. 
  int fd = sv[0];
  // Check if if there is input on fd
  // EPOLLIN check if there is input for the fd 
  ev.events = EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLET;            
  ev.data.fd = fd;

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
    if (evlist[j].events & EPOLLHUP) {
      printf("EPOLLHUP happened \n");
    }
    // If the peer close the connection. 
    if (evlist[j].events & EPOLLRDHUP) {
      printf("EPOLLRDHUP happened \n");
    }

    // If output happened to the fd. 
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT happened \n");
    }

    // If input happened to the fd. 
    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN happened\n");
    }            

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
    if (evlist[j].events & EPOLLRDHUP) {
      printf("EPOLLRDHUP happened \n");
    }
    // We didn't register this... How does this happen?
    if (evlist[j].events & EPOLLHUP) {
      printf("EPOLLHUP happened \n");
    }

    // If output happened to the fd. 
    if (evlist[j].events & EPOLLOUT) {
      printf("EPOLLOUT happened \n");
    }

    // If input happened to the fd. 
    if (evlist[j].events & EPOLLIN) {
      printf("EPOLLIN happened\n");
      int s = read(evlist[j].data.fd, buf, MAX_BUF);
      if (s == -1) {
        printf("read failed\n");
        return -1;
      }
      printf("read %d bytes: %.*s\n", s, s, buf);
    }            

  }
  return 0;
}
