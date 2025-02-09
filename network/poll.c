#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "poll.h"
#include "proto.h"

client_state_t client_states[MAX_CLIENTS] = {0};

void init_clients() {

  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_states[i].fd = -1;
    client_states[i].state = STATE_IDLE;
    memset(&client_states[i].buffer, '\0', BUFSIZE);
  }
}

void handle_client(int fd) {

  proto_header_t hdr = {0};

  hdr.type = htons(1); // pack the type
  hdr.version = htons(10);
  hdr.len = htons(sizeof(int)); // pack the len
  send(fd, &hdr, sizeof(proto_header_t), 0);
}

int find_free_slot() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_states[i].fd == -1) {
      return i;
    }
  }
  return -1;
}

// can we use a map? hello c ?
int find_slot_by_fd(int fd) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (fd == client_states[i].fd == client_states[i].fd) {
      return i;
    }
  }

  return -1;
}

int main() {

  struct sockaddr_in server_addr, client_addr;
  int server_fd, conn_fd, free_slot, opt = 1, nfds = 1;
  socklen_t client_len = sizeof(client_addr);
  struct pollfd fds[MAX_CLIENTS + 1];

  init_clients();

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return -1;
  };

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  }

  // listen
  if (listen(server_fd, LISTEN_CONN) == -1) {
    perror("listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d \n", PORT);

  memset(fds, 0, sizeof(fds));
  fds[0].fd = server_fd;
  fds[0].events = POLLIN;
  nfds = 1;

  while (1) {

    int ii = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_states[i].fd == -1) {
        continue;
      }

      fds[ii].fd = client_states[i].fd;
      fds[ii].events = POLLIN;
      ii++;
    }

    int n_events = poll(fds, nfds, -1);

    if (n_events == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }

    // this is like saying is this revents a pollin, weird syntax i guess?
    if (fds[0].revents & POLLIN) {
      if ((conn_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                            &client_len)) == -1) {
        perror("accept");
        continue;
      }
      printf("New Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr),
             ntohs(client_addr.sin_port));

      free_slot = find_free_slot();
      if (free_slot == -1) {
        printf("Server is full, closing new connection");
        close(conn_fd);
      } else {
        client_states[free_slot].fd = conn_fd;
        client_states[free_slot].state = STATE_CONNECTED;
        nfds++;
        printf("Slot %d has fd %d\n", free_slot, conn_fd);
      }

      n_events--;
    }

    for (int i = 1; i <= nfds && n_events > 0; i++) {
      if (fds[i].revents & POLLIN) {
        n_events--;

        int curr_fd = fds[i].fd;
        int curr_slot = find_slot_by_fd(curr_fd);
        ssize_t bytes_read =
            read(curr_fd, &client_states[curr_slot].buffer, BUFSIZE);

        if (0 < bytes_read) {
          printf("Successfully recieved data from the client:\t%s\n",
                 client_states[curr_slot].buffer);
          continue;
        }

        close(curr_fd);
        if (-1 == curr_slot) {
          printf("Tried to close an fd that does not exit???\n");
          continue;
        }

        client_states[curr_slot].fd = -1;
        client_states[curr_slot].state = STATE_DISCONNECTED;
        printf("Client disconnected or we encounteder an error\n");
        nfds--;
      }
    }
  }

  return 0;
}
