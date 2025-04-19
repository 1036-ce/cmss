#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include "ring_buffer.h"
#include "msg_queue.h"

typedef struct shared_data_t {
  int sid;
  sem_t sid_mtx;
  ring_buffer_t ring_bufs[MAX_PROCESS_CNT];
  msg_queue_t msg_que;
} shared_data_t;

void shared_data_init(const int slave_cnt, shared_data_t* shada);

void shared_data_dtor(const int slave_cnt, shared_data_t* shada);
