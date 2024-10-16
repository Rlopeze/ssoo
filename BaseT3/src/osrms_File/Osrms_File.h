#pragma once
#include <stdint.h>

typedef struct
{
  int process_id;
  char file_name[15];
  uint32_t size;
  uint32_t virtual_address;
} osrmsFile;

int os_read_file(osrmsFile *file_desc, char *dest);