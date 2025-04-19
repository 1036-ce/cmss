#pragma once

#include <stdbool.h>

#include "shared_data.h"
#include "message.h"
#include "seed_info.h"

void master_send_one(int sid, shared_data_t* shada, const mss_message_t *msg, const int len);

void master_send_all(int slave_cnt, shared_data_t* shada, const mss_message_t *msg, const int len);

bool master_try_fetch(msg_queue_t* msg_que, mss_message_t* msg);

void master_fetch(msg_queue_t* msg_que, mss_message_t* msg);

char* get_slave_path(int argc, char **argv);

char** gen_slave_argv(int argc, char **argv);

bool slave_try_fetch(ring_buffer_t* ring_buf, mss_message_t* msg);

void slave_fetch(ring_buffer_t* ring_buf, mss_message_t* msg);

void slave_send(msg_queue_t* msg_que, const mss_message_t *msg, const int len);

void slave_send_args_ensure(msg_queue_t* msg_que, int sid);

void slave_send_seed_info(msg_queue_t* msg_que, int sid, const seed_info_t* seed_info);

int get_sid(shared_data_t* shada);

ring_buffer_t* get_local_ring_buf(int sid, shared_data_t* shada);
