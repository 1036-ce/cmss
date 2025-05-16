#pragma once

#include "message.h"
#include "msg_queue.h"
#include "shared_data.h"
#include "seed_info.h"

//=================== slave utils ====================

bool slave_try_fetch(ring_buffer_t* ring_buf, mss_message_t* msg);

void slave_fetch(ring_buffer_t* ring_buf, mss_message_t* msg);

void slave_send(msg_queue_t* msg_que, const mss_message_t *msg, const int len);

void slave_send_args_ensure(msg_queue_t* msg_que, int sid);

void slave_send_seed_info(msg_queue_t* msg_que, int sid, const seed_info_t* seed_info);

// bool slave_update_cvg_map(cvg_map_t* cvg_map, const uint8_t* trace_bits);
bool slave_update_cvg_map(cvg_map_t* cvg_map, const uint8_t* virgin_map);

int get_sid(shared_data_t* shada);

ring_buffer_t* get_local_ring_buf(int sid, shared_data_t* shada);
