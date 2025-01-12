#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "proto.h"

void handle_client(int fd) {

  proto_header_t hdr = {0};

  hdr.type = htons(1); // pack the type
  hdr.version = htons(10);
  hdr.len = htons(sizeof(int)); // pack the len
  send(fd, &hdr, sizeof(proto_header_t), 0);
}

int main() {

  struct sockaddr_in socket_info = {0};
  socket_info.sin_family = AF_INET;
  socket_info.sin_addr.s_addr = INADDR_ANY;
  socket_info.sin_port = htons(5555);

  struct sockaddr_in *connection_info = {0};
  socklen_t addrlen = sizeof(struct sockaddr_in);

  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1) {
    perror("socket");
    return -1;
  }

  // bind
  if (bind(fd, (struct sockaddr *)&socket_info, sizeof(struct sockaddr)) ==
      -1) {
    perror("bind");
    close(fd);
    return -1;
  }

  // listen
  if (listen(fd, 0) == -1) {
    perror("listen");
    close(fd);
    return -1;
  }

  int i = 5;
  while (1) {
    int conn_fd = accept(fd, (struct sockaddr *)&connection_info, &addrlen);
    if (conn_fd == -1) {
      perror("accept");
      close(fd);
      return -1;
    }

    handle_client(conn_fd);

    i--;
    close(conn_fd);
  }

  close(fd);
  return 0;
}
