#include "ring_buffer.h"


void ring_buf_push(ring_buffer_t *ring_buf, const char* data, const int len);

void ring_buf_pop(ring_buffer_t *ring_buf, const int len);

void ring_buf_front_n(ring_buffer_t *ring_buf, const int n, char *data);

void ring_buf_push_int(ring_buffer_t *ring_buf, int val);

void ring_buf_get_int(ring_buffer_t *ring_buf, int *val);

int ring_buf_size_impl(ring_buffer_t *ring_buf);

int ring_buf_capacity(const ring_buffer_t *ring_buf);


void ring_buf_init(ring_buffer_t *ring_buf) {
  sem_init(&ring_buf->mutex, 1, 1);
  sem_init(&ring_buf->msg_cnt_sem, 1, 0);
  ring_buf->head = 0;
  ring_buf->tail = 0;
}

bool ring_buf_push_msg(ring_buffer_t *ring_buf, const char* data, const int len) {
  sem_wait(&ring_buf->mutex);

  if (ring_buf_size_impl(ring_buf) + len + sizeof(len) > ring_buf_capacity(ring_buf)) {
    // fmt::println(stderr, "can not push msg, size is {}, it is too big, current size: {}", len + sizeof(len), ring_buf_size_impl(ring_buf));
    fprintf(stderr, "can not push msg");
    sem_post(&ring_buf->mutex);
    return false;
  }

  ring_buf_push_int(ring_buf, len);
  ring_buf_push(ring_buf, data, len);

  sem_post(&ring_buf->msg_cnt_sem);
  sem_post(&ring_buf->mutex);

  return true;
}

void ring_buf_get_msg(ring_buffer_t *ring_buf, char *data, int *len) {
  sem_wait(&ring_buf->msg_cnt_sem);
  sem_wait(&ring_buf->mutex);

  ring_buf_get_int(ring_buf, len);
  ring_buf_front_n(ring_buf, *len, data);
  ring_buf_pop(ring_buf, *len);

  sem_post(&ring_buf->mutex);
}

bool ring_buf_try_get_msg(ring_buffer_t *ring_buf, char *data, int *len) {
  int status = sem_trywait(&ring_buf->msg_cnt_sem);
  if (status == -1) {
    return false;
  }
  sem_wait(&ring_buf->mutex);

  ring_buf_get_int(ring_buf, len);
  ring_buf_front_n(ring_buf, *len, data);
  ring_buf_pop(ring_buf, *len);

  sem_post(&ring_buf->mutex);
  return true;
}

int ring_buf_size(ring_buffer_t* ring_buf) {
  sem_wait(&ring_buf->mutex);

  int size = ring_buf_size_impl(ring_buf);

  sem_post(&ring_buf->mutex);
  return size;
}

void ring_buf_dtor(ring_buffer_t* ring_buf) {
  sem_destroy(&ring_buf->mutex);
  sem_destroy(&ring_buf->msg_cnt_sem);
}

void ring_buf_dbg(ring_buffer_t *ring_buf, char *str) {
  sem_wait(&ring_buf->mutex);

  int msg_cnt = 0;
  sem_getvalue(&ring_buf->msg_cnt_sem, &msg_cnt);

  sprintf(str, "head: %d, tail: %d\n", ring_buf->head, ring_buf->tail);
  sprintf(str, "message count: %d, mesage size: %d\n", msg_cnt, ring_buf_size_impl(ring_buf));

  sem_post(&ring_buf->mutex);
}

// below is private function

bool ring_buf_empty(ring_buffer_t* ring_buf) {
  sem_wait(&ring_buf->mutex);
  bool res = (ring_buf->tail == ring_buf->head);
  sem_post(&ring_buf->mutex);
  return res;
}

void ring_buf_push(ring_buffer_t *ring_buf, const char* data, const int len) {
  int pos = ring_buf->tail;
  for (int i = 0; i < len; ++i, ++pos) {
    if (pos == RING_BUF_SIZE) {
      pos = 0;
    }
    ring_buf->data[pos] = data[i];
  }
  ring_buf->tail = (ring_buf->tail + len) % RING_BUF_SIZE;
}

void ring_buf_pop(ring_buffer_t *ring_buf, const int len) {
  ring_buf->head = (ring_buf->head + len) % RING_BUF_SIZE;
}

void ring_buf_front_n(ring_buffer_t *ring_buf, const int n, char *data) {
  int pos = ring_buf->head;
  for (int i = 0; i < n; ++i, ++pos) {
    if (pos == RING_BUF_SIZE) {
      pos = 0;
    }
    data[i] = ring_buf->data[pos];
  }
}

void ring_buf_push_int(ring_buffer_t* ring_buf, int val) {
  ring_buf_push(ring_buf, (const char*)(&val), sizeof(int));
}

void ring_buf_get_int(ring_buffer_t *ring_buf, int *val) {
  ring_buf_front_n(ring_buf, sizeof(int), (char*)(val));
  ring_buf_pop(ring_buf, sizeof(int));
}

int ring_buf_size_impl(ring_buffer_t* ring_buf) {
  if (ring_buf->tail >= ring_buf->head) {
    return ring_buf->tail - ring_buf->head;
  }
  return  ring_buf->tail - ring_buf->head + RING_BUF_SIZE;
}

int ring_buf_capacity(const ring_buffer_t* ring_buf) {
  return RING_BUF_SIZE - 1;
}

