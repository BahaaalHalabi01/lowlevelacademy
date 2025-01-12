#ifndef PROTO_H
#define PROTO_H


#ifndef MAX_CLIENTS
#define MAX_CLIENTS 128
#endif

#ifndef PORT
#define PORT 5555
#endif
#ifndef BUFSIZE
#define BUFSIZE 4096
#endif

// typedef enum {
//   PROTO_HELLO,
// } proto_type_e;

typedef struct {
  unsigned int type;
  unsigned int len;
  unsigned int version;
} proto_header_t;

typedef enum {
  STATE_IDLE,
  STATE_CONNECTED,
  STATE_DISCONNECTED

} conn_state_t;

typedef struct {
  int fd;
  conn_state_t state;
  char buffer[BUFSIZE];
} client_state_t;


#endif
