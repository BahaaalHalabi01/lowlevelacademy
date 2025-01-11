#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <socket address>", argv[0]);
    return 0;
  }

  struct sockaddr_in socket_info = {0};
  socket_info.sin_family = AF_INET;
  socket_info.sin_addr.s_addr = inet_addr(argv[1]);
  socket_info.sin_port = htons(5555);

  socklen_t sock_size = sizeof(struct sockaddr);

  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1) {
    perror("socket");
    return -1;
  }

  int conn_fd = connect(fd, (struct sockaddr *)&socket_info, sock_size);
  if (conn_fd == -1) {

    perror("connect");
    close(fd);
    return -1;
  }

  close(fd);
}
