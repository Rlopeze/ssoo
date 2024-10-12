#pragma once
#include "../osrms_File/Osrms_File.h"

void os_mount(char *memory_path);

void os_ls_processes();

int os_exists(int process_id, char *file_name);

void os_ls_files(int process_id);

void os_frame_bitmap();

void os_tp_bitmap();