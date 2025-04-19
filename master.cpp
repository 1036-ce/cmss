#include <sys/stat.h>
#include <openssl/sha.h>

#include "master.h"

void log_init() {
	FILE *fp = fopen("master.log", "a");
	log_set_level(LOG_ERROR);
	log_add_fp(fp, LOG_TRACE);
}

std::string get_sha512_str(const char *str, const int len) {
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512((const unsigned char *)str, len, hash);
	return std::string{(char *)hash};
}

void master_sync_seed_handler(
    int slave_cnt, shared_data_t *shada,
    std::unordered_map<std::string, seed_info_t> &seed_pool,
    const mss_message_t &receive_msg) {

	seed_info_t seed_info;
  memset(&seed_info, 0, sizeof(seed_info_t));
	memcpy(&seed_info, receive_msg.data, receive_msg.len);
	log_info("master: receive SYNC_SEED from slave %d, "
	         "seed_file_name is: %s",
	         receive_msg.from, seed_info.seed_file_name);

	int fd = open(seed_info.seed_file_name, O_RDONLY);
	struct stat file_stat;
	if (fstat(fd, &file_stat) == -1) {
		perror("fstat");
		exit(1);
	}
	char *str =
	    (char *)mmap(nullptr, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

  std::string sha512 = get_sha512_str(str, file_stat.st_size);
	munmap(str, file_stat.st_size);
	close(fd);

  if (seed_pool.count(sha512) == 0) {
    seed_pool[sha512] = seed_info;
  }
  else {
    log_info("seed: %s has existed in global seed pool", seed_info.seed_file_name);
    return;
  }


	mss_message_t send_msg;
	send_msg.from = -1;
	send_msg.to   = -2;
	send_msg.cmd  = SYNC_SEED;
	send_msg.len  = receive_msg.len;
	memcpy(send_msg.data, &seed_info, receive_msg.len);
	master_send_all_except(slave_cnt, receive_msg.from, shada, &send_msg,
	                       message_real_length(&send_msg));
	log_info("master: send all other slaves SYNC_SEED");

	send_msg.from = -1;
	send_msg.to   = receive_msg.from;
	send_msg.cmd  = SEED_ENSURE;
	send_msg.len  = 0;
	master_send_one(receive_msg.from, shada, &send_msg,
	                message_real_length(&send_msg));
	log_info("master: send slave %d SEED_ENSURE", receive_msg.from);

}


void master_send_one(int sid, shared_data_t* shada, const mss_message_t *msg, const int len) {
  ring_buffer_t *ring_buf = &shada->ring_bufs[sid];
  ring_buf_push_msg(ring_buf, (const char*)msg, len);
}

void master_send_all(int slave_cnt, shared_data_t* shada, const mss_message_t *msg, const int len) {
  for (int sid = 0; sid < slave_cnt; ++sid) {
    master_send_one(sid, shada, msg, len);
  }
}

void master_send_all_except(int slave_cnt, int sid, shared_data_t* shada, const mss_message_t *msg, const int len) {
  for (int id = 0; id < slave_cnt; ++id) {
    if (id == sid) {
      continue;
    }
    master_send_one(id, shada, msg, len);
  }
}

bool master_try_fetch(msg_queue_t* msg_que, mss_message_t* msg) {
  int len;
  bool success = msg_queue_try_get_msg(msg_que, (char*)msg, &len);
  return success;
}

void master_fetch(msg_queue_t* msg_que, mss_message_t* msg) {
  int len;
  msg_queue_get_msg(msg_que, (char*)msg, &len);
}

char* get_slave_path(int argc, char **argv) {
  char *path = realpath(*(argv + 1), NULL);
  return path;
}

char** gen_slave_argv(int argc, char **argv) {
  char** slave_argv = (char**)malloc(sizeof(char*) * argc);

  for (int i = 1; i < argc; ++i) {
    slave_argv[i - 1] = argv[i];
  }
  slave_argv[argc] = NULL;

  return slave_argv;
}

void master_dir_init() {
}
