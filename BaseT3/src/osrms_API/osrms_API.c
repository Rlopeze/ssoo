#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"

char *path;
void read_pcb(FILE *file, int index, PCB *pcb)
{
  int offset = index * 256;
  fseek(file, offset, SEEK_SET);
  fread(pcb, sizeof(PCB), 1, file);
}

int find_process_offset(FILE *file, int process_id)
{
  PCB pcb;
  for (int i = 0; i < 32; i++)
  {
    read_pcb(file, i, &pcb);
    if (pcb.process_id == process_id)
    {
      return i * 256;
    }
  }
  return -1;
}

void os_mount(char *memory_path)
{
  path = memory_path;
}

void os_ls_processes()
{
  FILE *file = fopen(path, "rb");

  PCB pcb;
  for (int i = 0; i < 32; i++)
  {
    read_pcb(file, i, &pcb);
    if (pcb.state == 0x01)
    {
      printf("ID: %d, Name: %s\n", pcb.process_id, pcb.process_name);
    }
  }

  fclose(file);
}

int os_exists(int process_id, char *file_name)
{
  FILE *file = fopen(path, "rb");

  int process_offset = find_process_offset(file, process_id);
  if (process_offset == -1)
  {
    fclose(file);
    return 0;
  }

  PCB pcb;
  fseek(file, process_offset, SEEK_SET);
  fread(&pcb, sizeof(PCB), 1, file);

  for (int j = 0; j < 5; j++)
  {
    if (pcb.file_table[j].valid == 0x01 &&
        strcmp(pcb.file_table[j].file_name, file_name) == 0)
    {
      fclose(file);
      return 1;
    }
  }

  fclose(file);
  return 0;
}

void os_ls_files(int process_id)
{
  FILE *file = fopen(path, "rb");

  int process_offset = find_process_offset(file, process_id);
  if (process_offset == -1)
  {
    printf("Process ID %d not found.\n", process_id);
    fclose(file);
    return;
  }

  PCB pcb;
  fseek(file, process_offset, SEEK_SET);
  fread(&pcb, sizeof(PCB), 1, file);

  for (int j = 0; j < 5; j++)
  {
    if (pcb.file_table[j].valid == 0x01)
    {
      uint64_t file_size = 0;
      for (int k = 0; k < 4; k++)
      {
        file_size |= ((uint64_t)pcb.file_table[j].size_bytes[k] << (k * 8));
      }

      printf("Name: %s, Size: %lu bytes\n",
             pcb.file_table[j].file_name, file_size);
    }
  }
  fclose(file);
}
