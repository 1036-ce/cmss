#pragma once

#include "cmss_config.h"

typedef struct {
  char seed_file_name[FILE_NAME_SIZE];
} seed_info_t;

void seed_info_set_file_name(const char* file_name, const int len, seed_info_t* seed_info);
