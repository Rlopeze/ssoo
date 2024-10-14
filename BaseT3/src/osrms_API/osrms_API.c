#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"

char *path;

void os_mount(char *memory_path)
{
  path = memory_path;
}

void os_ls_processes()
{
  FILE *file = fopen(path, "rb");

  unsigned char byte;
  unsigned char process_name[15];
  unsigned char process_id;

  for (int i = 0; i < 32; i++)
  {
    int offset = i * 256;
    fseek(file, offset, SEEK_SET);
    fread(&byte, sizeof(unsigned char), 1, file);

    if (byte == 0x01)
    {
      fread(&process_id, sizeof(unsigned char), 1, file);
      fread(process_name, sizeof(unsigned char), 14, file);
      process_name[14] = '\0';
      printf("ID: %d, Nombre: %s\n", process_id, process_name);
    }
  }
  fclose(file);
}

int find_process_offset(FILE *file, int process_id)
{
  unsigned char current_process_id;

  for (int i = 0; i < 32; i++)
  {
    int offset = i * 256;
    fseek(file, offset, SEEK_SET);

    fseek(file, offset + 1, SEEK_SET);
    fread(&current_process_id, sizeof(unsigned char), 1, file);

    if ((int)current_process_id == process_id)
    {
      return offset;
    }
  }
  return -1;
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

  for (int j = 0; j < 5; j++)
  {
    unsigned char valid_file;
    fseek(file, process_offset + 13 + j * 23, SEEK_SET);
    fread(&valid_file, sizeof(unsigned char), 1, file);
    if (valid_file)
    {
      unsigned char file_name_read[14];
      fread(file_name_read, sizeof(unsigned char), 14, file);
      if (strcmp(file_name, file_name_read) == 0)
      {
        fclose(file);
        return 1;
      }
    }
  }

  fclose(file);
  return 0;
}