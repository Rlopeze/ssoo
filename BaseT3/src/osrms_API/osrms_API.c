#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"
#include "../osrms_File/Osrms_File.h"

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

uint32_t obtain_physical_address(uint32_t frame_number, uint16_t offset)
{

  uint32_t base_frames = (8 * 1024) + 128 + (128 * 1024) + (8 * 1024);
  return base_frames + (frame_number * 32768) + offset;
}

uint32_t obtain_pfn(uint16_t vpn)
{
  uint8_t first_level_index = vpn >> 6;
  uint8_t second_level_index = vpn & 0x3F;

  uint32_t table_address = 8 * 1024 + 128 + (first_level_index * 128);

  uint32_t entry_address = table_address + (second_level_index * 2);

  FILE *mem_file = fopen(path, "rb");

  fseek(mem_file, entry_address, SEEK_SET);
  uint16_t pfn;
  fread(&pfn, 8 * 1024 + 128, 1, mem_file);
  fclose(mem_file);

  return pfn;
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

int os_read_file(osrmsFile *file_desc, char *dest_path)
{
  FILE *mem_file = fopen(path, "rb");

  FILE *dest_file = fopen(dest_path, "wb");

  int total_bytes_read = 0;
  uint32_t virtual_addr = file_desc->virtual_address;
  uint32_t size = file_desc->size;

  while (total_bytes_read < size)
  {
    uint16_t vpn = (virtual_addr & 0xFFF000) >> 15;
    uint16_t offset = virtual_addr & 0x7FFF;

    uint32_t frame_number = obtain_pfn(vpn);

    uint32_t frame_pos = obtain_physical_address(frame_number, offset);

    fseek(mem_file, frame_pos, SEEK_SET);

    int bytes_to_read = 32768 - offset;
    if (bytes_to_read > (size - total_bytes_read))
    {
      bytes_to_read = size - total_bytes_read;
    }

    char buffer[32768];
    fread(buffer, 1, bytes_to_read, mem_file);
    fwrite(buffer, 1, bytes_to_read, dest_file);

    total_bytes_read += bytes_to_read;
    virtual_addr += bytes_to_read;
  }

  fclose(mem_file);
  fclose(dest_file);
  return total_bytes_read;
}

void os_close(osrmsFile *file_desc)
{
  free(file_desc);
}