#pragma once

#include "cmss_config.h"
#include "cmd.h"

typedef struct {
  cmd_t cmd;
  int from;  // -1 for master, other is sid
  int to;    // -1 for master, other is sid
  int num;   // for test
  int len;
  char data[MESSAGE_SIZE];
} mss_message_t;

int message_real_length(const mss_message_t* msg);

void message_to_hex(const mss_message_t* msg);
