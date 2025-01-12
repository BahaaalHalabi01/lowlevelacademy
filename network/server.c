#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
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

    // need to add server fd
    FD_SET(server_fd, &read_fds);
    nfds = server_fd + 1;

    // add active clients to set
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_states[i].fd != -1) {
        FD_SET(client_states[i].fd, &read_fds);
        if (client_states[i].fd > nfds)
          nfds = client_states[i].fd + 1;
      }
    }

    if (select(nfds, &read_fds, &write_fds, NULL, NULL) == -1) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    if (FD_ISSET(server_fd, &read_fds)) {
      if (-1 == (conn_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                                  &addrlen))) {
        perror("accept");
        continue;
      }

      printf("New connection frow %s:%d\n", inet_ntoa(client_addr.sin_addr),
             ntohs(client_addr.sin_port));

      if (-1 == (free_slot = find_free_slot())) {
        printf("Server is full,terminting new connection. with address %s\n",
               inet_ntoa(client_addr.sin_addr));
        close(conn_fd);
      } else {
        memset(&client_states[free_slot].buffer, '\0', BUFSIZE);
        client_states[free_slot].fd = conn_fd;
        client_states[free_slot].state = STATE_CONNECTED;
      }
    };

    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_states[i].fd != -1 &&
          FD_ISSET(client_states[i].fd, &read_fds)) {
        if (client_states[i].state != STATE_CONNECTED) {
          continue;
        }
        ssize_t bytes_read =
            recv(client_states[i].fd, &client_states[i].buffer, BUFSIZE, 0);

        if (bytes_read <= 0) {
          perror("recv");
          close(client_states[i].fd);
          client_states[i].fd = -1;
          client_states[i].state = STATE_IDLE;
          printf("Client disconnected or could not read from the socket\n");
        } else {
          printf("Received %ld bytes from client %s\n", bytes_read,
                 inet_ntoa(client_addr.sin_addr));
        }
      }
    }

    // handle_client(conn_fd);
  }

  close(server_fd);
  close(conn_fd);
  return 0;
}
