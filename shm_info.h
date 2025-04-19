#pragma once
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
  int fd;
  int size;
  char *data;
  const char* name;
} shm_info_t;


void shm_init(const char* name, const int size, shm_info_t *shm_info);

void shm_lookup(const char* name, shm_info_t *shm_info);

void shm_send(shm_info_t* const shm_info, const char* data, const int len);

// data must be big enough to receive
void shm_receive(shm_info_t* const shm_info, char* data, int *len);

void shm_clear(shm_info_t* const shm_info);

void shm_dtor(shm_info_t *shm_info);
