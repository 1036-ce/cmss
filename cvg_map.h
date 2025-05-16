#pragma once

#include <semaphore.h>
#include <stdint.h>

#include "cmss_config.h"

typedef struct {
  // sem_t mutex;
  uint8_t data[CVG_MAP_SIZE];
} cvg_map_t;

void cvg_map_init(cvg_map_t* cvg_map);

uint64_t cvg_map_get64(cvg_map_t* cvg_map, int pos);

uint8_t cvg_map_get(cvg_map_t* cvg_map, int pos);

void cvg_map_set64(cvg_map_t* cvg_map, int pos, uint64_t val);

void cvg_map_set(cvg_map_t* cvg_map, int pos, uint8_t val);

void cvg_map_and_at(cvg_map_t* cvg_map, int pos, uint8_t val);

void cvg_map_and_at64(cvg_map_t* cvg_map, int pos, uint64_t val);

void cvg_map_or_at(cvg_map_t* cvg_map, int pos, uint8_t val);

void cvg_map_or_at64(cvg_map_t* cvg_map, int pos, uint64_t val);

void cvg_map_dump(cvg_map_t* cvg_map, const char* fname);

void cvg_map_dtor(cvg_map_t* cvg_map);
