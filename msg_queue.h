#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>

#include "cmss_config.h"
#include "log.h"

typedef struct msg_queue_t {
  sem_t mutex;  // write lock
  sem_t msg_cnt_sem; // message count semaphore
  int head;
  int tail;
  char data[MSG_QUEUE_SIZE];
} msg_queue_t;


void msg_queue_init(msg_queue_t *msg_que);

bool msg_queue_push_msg(msg_queue_t *msg_que, const char* msg, const int len);

void msg_queue_get_msg(msg_queue_t *msg_que, char* msg, int *len);

bool msg_queue_try_get_msg(msg_queue_t *msg_que, char *msg, int *len);

void msg_queue_dtor(msg_queue_t *msg_que);

void msg_queue_dbg(msg_queue_t *msg_que, char *str);

void msg_queue_dump(msg_queue_t *msg_que);
