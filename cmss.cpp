#include <fcntl.h>
#include <limits.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>

extern "C" {
#include "args.h"
#include "cmss_config.h"
#include "log.h"
#include "message.h"
#include "msg_queue.h"
#include "shared_data.h"
#include "shm_info.h"
}

#include "master.h"

// indicates whether the program should stop
bool should_stop = false;

static void handle_stop_sig(int sig) {
	should_stop = true;
}

void sig_setup() {
	struct sigaction sa;

	sa.sa_handler   = NULL;
	sa.sa_flags     = SA_RESTART;
	sa.sa_sigaction = NULL;

	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_stop_sig;
	sigaction(SIGINT, &sa, NULL);
}


void server(int slave_cnt, shared_data_t *shada, shm_info_t *shm_info) {
	log_info("master: fuzzing start");
	msg_queue_t *msg_que = &(shada->msg_que);

	// key is sha512 string
	std::unordered_map<std::string, seed_info_t> seed_pool;
	mss_message_t receive_msg;
	mss_message_t send_msg;
	int len;

	send_msg.from = -1;
	send_msg.num  = 1;
	for (int i = 0; i < slave_cnt; ++i) {
		args_t args = args_common(i);
		memcpy(send_msg.data, &args, sizeof(args_t));
		send_msg.len = sizeof(args_t);
		send_msg.to  = i;
		send_msg.cmd = ARGS;
		master_send_one(i, shada, &send_msg, message_real_length(&send_msg));
		log_info("master: send args to slave %d", i);
	}

	while (true) {
		if (should_stop) {
			killpg(0, SIGINT);
      for (int i = 0; i < slave_cnt; ++i) {
        wait(NULL);
      }
			shared_data_dtor(slave_cnt, shada);
			shm_dtor(shm_info);
			log_info("master: fuzzing done");
			return;
		}
		if (master_try_fetch(msg_que, &receive_msg)) {
			// msg_queue_dump(msg_que);
			switch (receive_msg.cmd) {
			case ARGS_ENSURE: {
				log_info("master: receive ARGS_ENSURE from slave %d",
				         receive_msg.from);
				break;
			}
			case SEED_ENSURE: {
				log_info("master: receive SEED_ENSURE from slave %d",
				         receive_msg.from);
				break;
			}
			case SYNC_SEED: {
				master_sync_seed_handler(slave_cnt, shada, seed_pool,
				                         receive_msg);
				break;
			}
			default: {
				log_warn("master: receive wrong cmd from slave %d",
				         receive_msg.from);
				break;
			}
			}
		} else {
			sleep(1);
		}
	}
}

pid_t exec_fuzzer(char *slave_path, char **slave_argv) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }
  if (pid == 0) {
		execv(slave_path, slave_argv);
		perror("slave");
		exit(1);
  }
  return pid;
}

int main(int argc, char **argv) {

	sig_setup();
	log_init();

	int slave_cnt = 5;
	shm_info_t shm_info;
	shm_init(CMSS_SHM_NAME, sizeof(shared_data_t), &shm_info);
	shared_data_t *shada = (shared_data_t *)(shm_info.data);
	shared_data_init(slave_cnt, shada);
	char *slave_path  = get_slave_path(argc, argv);
	char **slave_argv = gen_slave_argv(argc, argv);

  for (int i = 0; i < slave_cnt; ++i) {
    exec_fuzzer(slave_path, slave_argv);
  }

  master_dir_init();
	server(slave_cnt, shada, &shm_info);
	free(slave_argv);

	return 0;
}
