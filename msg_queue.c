#include "msg_queue.h"
#include "log.h"

// Private function prototype

void msg_queue_push(msg_queue_t *msg_que, const char* data, const int len);

void msg_queue_pop(msg_queue_t *msg_que, const int len);

void msg_queue_front_n(msg_queue_t *msg_que, const int n, char *data);

void msg_queue_push_int(msg_queue_t *msg_que, int val);

void msg_queue_get_int(msg_queue_t *msg_que, int *val);

int msg_queue_size_impl(msg_queue_t *msg_que);

int msg_queue_capacity(const msg_queue_t *msg_que);


// Public function implementation

void msg_queue_init(msg_queue_t *msg_que) {
  sem_init(&msg_que->mutex, 1, 1);
  sem_init(&msg_que->msg_cnt_sem, 1, 0);
  msg_que->head = 0;
  msg_que->tail = 0;
}

bool msg_queue_push_msg(msg_queue_t *msg_que, const char* msg, const int len) {

  sem_wait(&msg_que->mutex);

  if (msg_queue_size_impl(msg_que) + len + sizeof(len) > msg_queue_capacity(msg_que)) {
    fprintf(stderr, "can not push msg");
    sem_post(&msg_que->mutex);
    return false;
  }

  msg_queue_push_int(msg_que, len);
  msg_queue_push(msg_que, msg, len);
  sem_post(&msg_que->msg_cnt_sem);
  sem_post(&msg_que->mutex);

  return true;
}

void msg_queue_get_msg(msg_queue_t *msg_que, char* msg, int *len) {
  sem_wait(&msg_que->msg_cnt_sem);
  sem_wait(&msg_que->mutex);

  msg_queue_get_int(msg_que, len);
  msg_queue_front_n(msg_que, *len, msg);
  msg_queue_pop(msg_que, *len);

  sem_post(&msg_que->mutex);
}

bool msg_queue_try_get_msg(msg_queue_t *msg_que, char *msg, int *len) {
  int status = sem_trywait(&msg_que->msg_cnt_sem);
  if (status == -1) {
    return false;
  }
  sem_wait(&msg_que->mutex);

  msg_queue_get_int(msg_que, len);
  msg_queue_front_n(msg_que, *len, msg);
  msg_queue_pop(msg_que, *len);

  sem_post(&msg_que->mutex);
  return true;
}

void msg_queue_dtor(msg_queue_t *msg_que) {
  sem_destroy(&msg_que->mutex);
  sem_destroy(&msg_que->msg_cnt_sem);
}

void msg_queue_dbg(msg_queue_t *msg_que, char *str) {
  sem_wait(&msg_que->mutex);

  int msg_cnt = 0;
  sem_getvalue(&msg_que->msg_cnt_sem, &msg_cnt);

  sprintf(str, "head: %d, tail: %d\n", msg_que->head, msg_que->tail);
  sprintf(str, "message count: %d, mesage size: %d\n", msg_cnt, msg_queue_size_impl(msg_que));

  sem_post(&msg_que->mutex);
}

void msg_queue_dump(msg_queue_t *msg_que) {
  sem_wait(&msg_que->mutex);

  int msg_cnt = 0;
  sem_getvalue(&msg_que->msg_cnt_sem, &msg_cnt);

  log_info("msg_que dump: head: %d, tail: %d, message count: %d", msg_que->head, msg_que->tail, msg_cnt);
  /* sprintf(str, "head: %d, tail: %d\n", msg_que->head, msg_que->tail);
   * sprintf(str, "message count: %d, mesage size: %d\n", msg_cnt, msg_queue_size_impl(msg_que)); */

  sem_post(&msg_que->mutex);
}

// Private function implementation

void msg_queue_push(msg_queue_t *msg_que, const char* data, const int len) {
  int pos = msg_que->tail;
  for (int i = 0; i < len; ++i, ++pos) {
    if (pos == MSG_QUEUE_SIZE) {
      pos = 0;
    }
    msg_que->data[pos] = data[i];
  }
  msg_que->tail = (msg_que->tail + len) % MSG_QUEUE_SIZE;
}

void msg_queue_pop(msg_queue_t *msg_que, const int len) {
  msg_que->head = (msg_que->head + len) % MSG_QUEUE_SIZE;
}

void msg_queue_front_n(msg_queue_t *msg_que, const int n, char *data) {
  int pos = msg_que->head;
  for (int i = 0; i < n; ++i, ++pos) {
    if (pos == MSG_QUEUE_SIZE) {
      pos = 0;
    }
    data[i] = msg_que->data[pos];
  }
}

void msg_queue_push_int(msg_queue_t *msg_que, int val) {
  msg_queue_push(msg_que, (const char*)&val, sizeof(int));
}

void msg_queue_get_int(msg_queue_t *msg_que, int *val) {
  msg_queue_front_n(msg_que, sizeof(int), (char*)val);
  msg_queue_pop(msg_que, sizeof(int));
}

int msg_queue_size_impl(msg_queue_t *msg_que) {
  if (msg_que->tail >= msg_que->head) {
    return msg_que->tail - msg_que->head;
  }
  return  msg_que->tail - msg_que->head + MSG_QUEUE_SIZE;
}

int msg_queue_capacity(const msg_queue_t *msg_que) {
  return MSG_QUEUE_SIZE - 1;
}
