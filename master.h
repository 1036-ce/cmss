#pragma once

#include <string>
#include <unordered_map>

extern "C" {
#include "args.h"
#include "cmss_config.h"
#include "log.h"
#include "message.h"
#include "msg_queue.h"
#include "shared_data.h"
#include "shm_info.h"
#include "seed_info.h"
}

//=================== master utils ====================

void log_init();

std::string get_sha512_str(const char *str, const int len);

void master_sync_seed_handler(
    int slave_cnt, shared_data_t *shada,
    std::unordered_map<std::string, seed_info_t> &seed_pool,
    const mss_message_t &receive_msg);

void master_send_one(int sid, shared_data_t* shada, const mss_message_t *msg, const int len);

void master_send_all(int slave_cnt, shared_data_t* shada, const mss_message_t *msg, const int len);

void master_send_all_except(int slave_cnt, int sid, shared_data_t* shada, const mss_message_t *msg, const int len);

bool master_try_fetch(msg_queue_t* msg_que, mss_message_t* msg);

void master_fetch(msg_queue_t* msg_que, mss_message_t* msg);

char* get_slave_path(int argc, char **argv);

char** gen_slave_argv(int argc, char **argv);

void master_dir_init();
