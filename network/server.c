#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "proto.h"

client_state_t client_states[MAX_CLIENTS] = {0};

void init_clients() {

  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_states[i].fd = -1;
    client_states[i].state = STATE_IDLE;
    memset(&client_states[i].buffer, '\0', BUFSIZE);
  }
}

int find_free_slot() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_states[i].fd == -1) {
      return i;
    }
  }
  return -1;
}

void handle_client(int fd) {

  proto_header_t hdr = {0};

  hdr.type = htons(1); // pack the type
  hdr.version = htons(10);
  hdr.len = htons(sizeof(int)); // pack the len
  send(fd, &hdr, sizeof(proto_header_t), 0);
}

int main() {

  struct sockaddr_in server_addr, client_addr;
  int server_fd, conn_fd, nfds, free_slot;
  socklen_t addrlen = sizeof(client_addr);
  fd_set read_fds, write_fds;

  init_clients();

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return -1;
  };

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // bind
  if (bind(server_fd, (struct sockaddr *)&server_addr,
           sizeof(struct sockaddr)) == -1) {
    perror("bind");
    close(server_fd);
    return -1;
  }

  // listen
  if (listen(server_fd, 0) == -1) {
    perror("listen");
    close(server_fd);
    return -1;
  }

  while (1) {

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    //need to add server fd
    FD_SET(server_fd, &read_fds);
    nfds = server_fd + 1;

    //add active clients to set
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_states[i].fd != -1) {
        FD_SET(client_states[i].fd, &read_fds);
        if (client_states[i].fd > nfds)
          nfds = client_states[i].fd + 1;
      }
    }

    conn_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (conn_fd == -1) {
      perror("accept");
      close(server_fd);
      return -1;
    }

    handle_client(conn_fd);

    close(conn_fd);
  }

  close(server_fd);
  return 0;
}
