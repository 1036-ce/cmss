#include "shm_info.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>



void shm_init(const char* name, const int size, shm_info_t *shm_info) {
  int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    perror("shm_open");
    exit(1);
  }

  ftruncate(fd, size);
  char *ptr = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  shm_info->fd = fd;
  shm_info->data = ptr;
  shm_info->size = size;
  shm_info->name = name;
  shm_clear(shm_info);
}

void shm_lookup(const char* name, shm_info_t *shm_info) {
  int fd = shm_open(name, O_RDWR, 0666);
  if (fd == -1) {
    perror("shm_open");
    exit(1);
  }

  struct stat shm_stat;
  if (fstat(fd, &shm_stat) == -1) {
    perror("fstat");
    exit(1);
  }
  int shm_size = shm_stat.st_size;

  char *ptr = (char*)mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  shm_info->fd = fd;
  shm_info->data = ptr;
  shm_info->size = shm_size;
  shm_info->name = name; 
}


void shm_send(shm_info_t* const shm_info, const char* data, const int len) {
  int total_len = sizeof(len) + len;
  if (total_len > shm_info->size) {
    // fmt::println(stderr, "msg is too big");
    fprintf(stderr, "msg is too big\n");
    exit(1);
  }

  char *ptr = shm_info->data;
  memcpy(ptr, &len, sizeof(len));
  ptr += sizeof(len);
  memcpy(ptr, data, len);
}

void shm_receive(shm_info_t* const shm_info, char* data, int *len) {
  char *ptr = shm_info->data;

  memcpy(len, ptr, sizeof(*len));
  ptr += sizeof(*len);

  memcpy(data, ptr, *len);
}

void shm_clear(shm_info_t* const shm_info) {
  memset(shm_info->data, 0, shm_info->size);
}

void shm_dtor(shm_info_t *shm_info) {
  munmap(shm_info->data, shm_info->size);
  close(shm_info->fd);
  shm_unlink(shm_info->name);
}

