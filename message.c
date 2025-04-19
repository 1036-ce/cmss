#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int message_real_length(const mss_message_t *msg) { return sizeof(mss_message_t) - MESSAGE_SIZE + msg->len; }

void message_to_hex(const mss_message_t *msg) {
  int len = message_real_length(msg);
  char *data = (char *)msg;

  for (int i = 0; i < len; ++i) {
    unsigned char val = *(data + i);
    printf("%02x ", val);
  }
  printf("\n");
}
