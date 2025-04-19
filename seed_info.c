#include <stdlib.h>
#include <string.h>
#include "seed_info.h"
#include "log.h"

void seed_info_set_file_name(const char* file_name, const int len, seed_info_t* seed_info) {
  if (len >= FILE_NAME_SIZE) {
    log_fatal("The seed file name is too long");
    exit(1);
  }
  memcpy(seed_info->seed_file_name, file_name, len);
  seed_info->seed_file_name[len] = '\0';
}
