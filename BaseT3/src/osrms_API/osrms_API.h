#pragma once
#include "../osrms_File/Osrms_File.h"

typedef struct
{
  uint8_t valid;
  char file_name[14];
  uint8_t size_bytes[4];
  uint8_t address_bytes[4];
} FileEntry;

typedef struct
{
  uint8_t state;
  uint8_t process_id;
  char process_name[11];
  FileEntry file_table[5];
} PCB;

void os_mount(char *memory_path);

void os_ls_processes();

int os_exists(int process_id, char *file_name);

void os_ls_files(int process_id);

void os_frame_bitmap();

void os_tp_bitmap();