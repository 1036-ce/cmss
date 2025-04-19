#pragma once

#include "cmss_config.h"

typedef struct {
  // -o
  char out_dir[ARGS_BUF_SIZE];
  // -d
  int skip_deterministic; // 0/1
  // -q
  int state_selection_algo;
  // -s
  int seed_selection_algo;
  // -E 
  int state_aware_mode; // 0/1
  // -K
  int terminate_child; // 0/1
  // -R
  int region_level_mutation; // 0/1
} args_t; 

args_t get_args(int argc, char **argv);

args_t args_common(int sid);

void args_dbg(const args_t* args);

char** get_slave_argv(int argc, char **argv);

