#pragma once
#include "../osrms_File/Osrms_File.h"

typedef struct
{
  unsigned char state;
  unsigned char process_id;
  char process_name[11];
  unsigned char valid[5];
  char file_name[5][14];
  unsigned int file_size[5];
  unsigned int virtual_address[5];
} PCB;

void os_mount(char *memory_path);

void os_ls_processes();

int os_exists(int process_id, char *file_name);

void os_ls_files(int process_id);

void os_frame_bitmap();

void os_tp_bitmap();