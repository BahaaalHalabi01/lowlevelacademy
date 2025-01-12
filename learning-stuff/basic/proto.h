#ifndef PROTO_H
#define PROTO_H

// typedef enum {
//   PROTO_HELLO,
// } proto_type_e;

typedef struct {
  unsigned int type;
  unsigned int len;
  unsigned int version;
} proto_header_t;

#endif
