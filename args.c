#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "args.h"

args_t args_common(int sid) {
  args_t args;
  sprintf(args.out_dir, "fuzzer%d", sid);
  args.skip_deterministic = 1;
  args.state_selection_algo = 3;
  args.seed_selection_algo = 3;
  args.terminate_child = 1;
  args.region_level_mutation = 1;
  return args;
}

void args_dbg(const args_t* args) {
  fprintf(stdout, "out_dir: %s\n", args->out_dir);
  fprintf(stdout, "skip_deterministic: %d\n", args->skip_deterministic);
  fprintf(stdout, "state_selection_algo: %d\n", args->state_selection_algo);
  fprintf(stdout, "seed_selection_algo: %d\n", args->seed_selection_algo);
  fprintf(stdout, "state_aware_mode: %d\n", args->state_aware_mode);
  fprintf(stdout, "terminate_child: %d\n", args->terminate_child);
  fprintf(stdout, "region_level_mutation: %d\n", args->region_level_mutation);
}
