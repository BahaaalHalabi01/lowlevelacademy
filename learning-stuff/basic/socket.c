#include <netinet/in.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

  struct sockaddr_in socket_info = {0};
  socket_info.sin_family = AF_INET;
  socket_info.sin_addr.s_addr = 0;
  socket_info.sin_port = htons(5555);

  struct sockaddr_in *connection_info = {0};
  socklen_t addrlen = 0;

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

  // accept
  int conn_fd = accept(fd, (struct sockaddr *)&connection_info, &addrlen);
  if (conn_fd == -1) {
    perror("accept");
    close(fd);
    return -1;
  }

  close(fd);
  close(conn_fd);
  return 0;
}
