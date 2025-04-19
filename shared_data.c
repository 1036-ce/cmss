#include "shared_data.h"

void shared_data_init(const int slave_cnt, shared_data_t* shada) {
  shada->sid = 0;
  sem_init(&shada->sid_mtx, 1, 1);
  for (int i = 0; i < slave_cnt; ++i) {
    ring_buf_init(&shada->ring_bufs[i]);
  }
  msg_queue_init(&shada->msg_que);
}

void shared_data_dtor(const int slave_cnt, shared_data_t* shada) {
  sem_destroy(&shada->sid_mtx);
  for (int i = 0; i < slave_cnt; ++i) {
    ring_buf_dtor(&shada->ring_bufs[i]);
  }
  msg_queue_dtor(&shada->msg_que);
}
