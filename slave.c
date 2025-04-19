#include <fcntl.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>

#include "cmss_config.h"
#include "utils.h"
#include "shared_data.h"
#include "shm_info.h"
#include "message.h"

void task() {
  sleep(2);
}

void args_dump(int argc, char **argv) {
  for (int i = 0; i < argc; ++i) {
    printf("%s ", argv[i]);
  }
  printf("\n");
}

int main(int argc, char **argv) {
  // init
  shm_info_t shm_info;
  shm_lookup(CMSS_SHM_NAME, &shm_info);
  shared_data_t* shada = (shared_data_t*)(shm_info.data);
  int sid = get_sid(shada);
  ring_buffer_t* ring_buf = get_local_ring_buf(sid, shada);
  msg_queue_t* msg_que = &(shada->msg_que);

  printf("slave %d: get sid: %d\n", sid, sid);
  args_dump(argc, argv);

  mss_message_t receive_msg;
  mss_message_t send_msg;

  while (true) {
    if (slave_try_fetch(ring_buf, &receive_msg)) {
      printf("slave %d: received msg from master, num: %d\n", sid, receive_msg.num);
      int num = receive_msg.num;
      send_msg.from = sid;
      send_msg.to = -1;
      send_msg.num = num + 1;
      send_msg.len = 0;
      slave_send(msg_que, &send_msg, message_real_length(&send_msg));
      printf("slave %d: send msg from slave %d to master, num: %d\n", sid, sid, send_msg.num);
    }
    else {
      fprintf(stderr, "slave %d : no msg\n", sid);
      task();
    }
  } 
}
