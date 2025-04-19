#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

extern "C" {
#include "args.h"
#include "cmss_config.h"
#include "log.h"
#include "message.h"
#include "msg_queue.h"
#include "shared_data.h"
#include "shm_info.h"
#include "utils.h"
}

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

void log_init() {
	FILE *fp = fopen("master.log", "a");
	log_set_level(LOG_ERROR);
	log_add_fp(fp, LOG_TRACE);
}

void server(int slave_cnt, shared_data_t *shada, shm_info_t *shm_info) {
	msg_queue_t *msg_que = &(shada->msg_que);
  msg_queue_dump(msg_que);

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
		log_info("master send args to slave %d", i);
	}

	while (true) {
		if (should_stop) {
			killpg(0, SIGINT);
			wait(NULL);
			shared_data_dtor(slave_cnt, shada);
			shm_dtor(shm_info);
			log_info("fuzzing done");
			return;
		}
		if (master_try_fetch(msg_que, &receive_msg)) {
      msg_queue_dump(msg_que);
			switch (receive_msg.cmd) {
			case ARGS_ENSURE: {
				log_info("master receive ARGS_ENSURE from slave %d",
				         receive_msg.from);
				break;
			}
			case SEED_ENSURE: {
				log_info("master receive SEED_ENSURE from slave %d",
				         receive_msg.from);
				break;
			}
			case SYNC_SEED: {
				seed_info_t seed_info;
				memcpy(&seed_info, receive_msg.data, receive_msg.len);
				log_info("master receive SYNC_SEED from slave %d, "
				         "seed_file_name is: %s",
				         receive_msg.from, seed_info.seed_file_name);
				break;
			}
			default: {
				log_warn("master receive wrong cmd from slave %d",
				         receive_msg.from);
				break;
			}
			}
			/*       printf("master: received msg from slave %d, num : %d\n",
			 * receive_msg.from, receive_msg.num);
			 *
			 *       send_msg.from = -1;
			 *       send_msg.to = receive_msg.from;
			 *       send_msg.num = receive_msg.num + 1;
			 *       send_msg.len = 0;
			 *       master_send_one(send_msg.to, shada, &send_msg,
			 * message_real_length(&send_msg)); */
		} else {
			// log_trace("master do not receive message form any slave");
			sleep(1);
		}
	}
}

int main(int argc, char **argv) {

	sig_setup();
	log_init();

	int slave_cnt = 3;
	shm_info_t shm_info;
	shm_init(CMSS_SHM_NAME, sizeof(shared_data_t), &shm_info);
	shared_data_t *shada = (shared_data_t *)(shm_info.data);
	shared_data_init(slave_cnt, shada);
	char *slave_path  = get_slave_path(argc, argv);
	char **slave_argv = gen_slave_argv(argc, argv);

	pid_t pid;
	for (int i = 0; i < slave_cnt; ++i) {
		pid = fork();
		if (pid == 0 || pid == -1) {
			break;
		}
	}
	if (pid == -1) {
		perror("fork");
		exit(1);
	}
	if (pid == 0) {
		execv(slave_path, slave_argv);
		perror("slave");
		exit(1);
	}

	server(slave_cnt, shada, &shm_info);
	free(slave_argv);

	return 0;
}
