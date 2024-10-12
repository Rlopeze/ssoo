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
