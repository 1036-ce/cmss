#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cvg_map.h"

void cvg_map_init(cvg_map_t* cvg_map) {
  memset(cvg_map->data, 0xff, CVG_MAP_SIZE);
  sem_init(&cvg_map->mutex, 1, 1);
}

uint64_t cvg_map_get64(cvg_map_t* cvg_map, int pos) {
  uint64_t ret;
  uint64_t *ptr = (uint64_t*)&(cvg_map->data[pos << 3]);
  __atomic_load(ptr, &ret, __ATOMIC_RELAXED);
  return ret;
}

uint8_t cvg_map_get(cvg_map_t* cvg_map, int pos) {
  char ret;
  __atomic_load(&(cvg_map->data[pos]), &ret, __ATOMIC_RELAXED);
  return ret;
}

void cvg_map_and_at(cvg_map_t* cvg_map, int pos, uint8_t val) {
  __atomic_and_fetch(&(cvg_map->data[pos]), val, __ATOMIC_RELAXED);
}

void cvg_map_and_at64(cvg_map_t* cvg_map, int pos, uint64_t val) {
  uint64_t *ptr = (uint64_t*)&(cvg_map->data[pos << 3]);
  __atomic_and_fetch(ptr, val, __ATOMIC_RELAXED);
}

void cvg_map_dump(cvg_map_t* cvg_map, const char* fname) {
  FILE *fp = fopen(fname, "w");
  fwrite(cvg_map->data, sizeof(char), CVG_MAP_SIZE, fp);
  // fprintf(fd, "%s", cvg_map->data);
  fclose(fp);
}

void cvg_map_dtor(cvg_map_t* cvg_map) {
  sem_destroy(&cvg_map->mutex);
}
