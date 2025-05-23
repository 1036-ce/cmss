#pragma once

#define VISIT_ARRAY_SIZE  4096
#define RING_BUF_SIZE  8192
#define CMD_SHM_SIZE  4096
#define MAX_PROCESS_CNT  128
//  MSG_QUEUE_SIZE = (MAX_PROCESS_CNT * RING_BUF_SIZE)
#define MSG_QUEUE_SIZE 1048576 
#define MESSAGE_SIZE  2048
#define ARGS_BUF_SIZE  64
#define FILE_NAME_SIZE 128

// CVG_MAP_SIZE = MAP_SIZE(config.h)
#define CVG_MAP_SIZE 65536

static const char *CMSS_SHM_NAME = "mss_shm";
static const char *MASTER_DIR_NAME = "master";
