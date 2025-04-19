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

typedef struct ring_buffer_t {
  sem_t mutex;  // write lock
  sem_t msg_cnt_sem; // message count semaphore
  int head;
  int tail;
  char data[RING_BUF_SIZE];
} ring_buffer_t;


// name is shm's name
void ring_buf_init(ring_buffer_t *ring_buf);

bool ring_buf_push_msg(ring_buffer_t *ring_buf, const char* data, const int len);

// success or block
void ring_buf_get_msg(ring_buffer_t *ring_buf, char *data, int *len);

bool ring_buf_try_get_msg(ring_buffer_t *ring_buf, char *data, int *len);

bool ring_buf_empty(ring_buffer_t* ring_buf);

int ring_buf_size(ring_buffer_t* ring_buf);

void ring_buf_dtor(ring_buffer_t* ring_buf);

void ring_buf_dbg(ring_buffer_t *ring_buf, char *str);
