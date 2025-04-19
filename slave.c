#include "slave.h"

bool slave_try_fetch(ring_buffer_t* ring_buf, mss_message_t* msg) {
  int len;
  bool success = ring_buf_try_get_msg(ring_buf, (char*)msg, &len);
  return success;
}

void slave_fetch(ring_buffer_t* ring_buf, mss_message_t* msg) {
  int len;
  ring_buf_get_msg(ring_buf, (char*)msg, &len);
}

void slave_send(msg_queue_t* msg_que, const mss_message_t *msg, const int len) {
  msg_queue_push_msg(msg_que, (const char*)msg, len);
}

void slave_send_args_ensure(msg_queue_t* msg_que, int sid) {
  mss_message_t msg;
  msg.from = sid;
  msg.to = -1;
  msg.cmd = ARGS_ENSURE;
  msg.len = 0;
  slave_send(msg_que, &msg, message_real_length(&msg));
}

void slave_send_seed_info(msg_queue_t* msg_que, int sid, const seed_info_t* seed_info) {
  mss_message_t msg;
  msg.from = sid;
  msg.to = -1;
  msg.cmd = SYNC_SEED;
  msg.len = sizeof(seed_info_t);
  memcpy(msg.data, seed_info, sizeof(seed_info_t));
  slave_send(msg_que, &msg, message_real_length(&msg));
}

int get_sid(shared_data_t* shada) {
  sem_wait(&shada->sid_mtx);
  int sid = shada->sid;
  ++shada->sid;
  sem_post(&shada->sid_mtx);
  return sid;
}

ring_buffer_t* get_local_ring_buf(int sid, shared_data_t* shada) {
  return &shada->ring_bufs[sid];
}
