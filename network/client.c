#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "proto.h"

void handle_connect(int fd) {

  proto_header_t hdr = {0};
  if (recv(fd, &hdr, sizeof(proto_header_t), 0) == -1) {
    perror("recv");
    return;
  }

  hdr.type = ntohs(hdr.type);       // unpack the type
  hdr.version = ntohs(hdr.version); // unpack the type
  hdr.len = ntohs(hdr.len);

  printf("type: %d\n", hdr.type);
  printf("version: %d\n", hdr.version);
  printf("len: %d\n", hdr.len);

  if (hdr.version != 10) {
    printf("Protocol mismatch! %d\n", hdr.version);
    return;
  }

  printf("Successfully connected to the server, protocol v1\n");

  return;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <socket address>", argv[0]);
    return 0;
  }

  struct sockaddr_in socket_info = {0};
  socket_info.sin_family = AF_INET;
  socket_info.sin_port = htons(5555);

  if (inet_pton(AF_INET, argv[1], &socket_info.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  socklen_t sock_size = sizeof(struct sockaddr);

  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1) {
    perror("socket");
    return -1;
  }

  if (connect(fd, (struct sockaddr *)&socket_info, sizeof(socket_info)) == -1) {
    perror("connect");
    close(fd);
    return -1;
  }

  handle_connect(fd);

  close(fd);
}
