#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"
#include "../osrms_File/Osrms_File.h"

char *path;
void read_pcb(FILE *file, int index, PCB *pcb)
{
  // print file, index, pcb
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

uint32_t obtain_pfn(uint16_t vpn) {
    uint8_t first_level_index = vpn >> 6;
    uint8_t second_level_index = vpn & 0x3F;
    uint32_t table_address = 8 * 1024 + 128 + (first_level_index * 128);
    uint32_t entry_address = table_address + (second_level_index * 2);

    FILE *mem_file = fopen(path, "rb");
    if (!mem_file) {
        printf("Error abriendo archivo de memoria en obtain_pfn.\n");
        return -1;
    }

    fseek(mem_file, entry_address, SEEK_SET);

    uint16_t pfn;
    size_t read_size = fread(&pfn, sizeof(uint16_t), 1, mem_file);
    if (read_size != 1) {
        printf("Error leyendo el PFN desde la tabla de páginas.\n");
        fclose(mem_file);
        return -1;
    }

    fclose(mem_file);

    return pfn;
}


void os_mount(char *memory_path)
{
  path = memory_path;
}

void os_ls_processes()
{
  // print path
  FILE *file = fopen(path, "rb");
  // print file
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

void os_frame_bitmap()
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        printf("Error opening memory file.\n");
        return;
    }
    int frame_bitmap_offset = (8 * 1024) + 128 + (128 * 1024);

    fseek(file, frame_bitmap_offset, SEEK_SET);
    uint8_t bitmap[8192];
    fread(bitmap, sizeof(uint8_t), 8192, file);

    int occupied = 0, free = 0;
    for (int i = 0; i < 65536; i++)
    {
        int bit = (bitmap[i / 8] >> (i % 8)) & 1;
        printf("%d", bit);
        if (bit == 1)
            occupied++;
        else
            free++;
    }

    printf("\nFrames ocupados: %d\n", occupied);
    printf("Frames libres: %d\n", free);

    fclose(file);
}


void os_tp_bitmap()
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        printf("Error opening memory file.\n");
        return;
    }

    int tp_bitmap_offset = 8 * 1024;
    fseek(file, tp_bitmap_offset, SEEK_SET);

    uint8_t bitmap[128];
    fread(bitmap, sizeof(uint8_t), 128, file);

    int occupied = 0, free = 0;
    for (int i = 0; i < 1024; i++)
    {
        int bit = (bitmap[i / 8] >> (i % 8)) & 1;

        printf("%d", bit);

        if (bit == 1)
            occupied++;
        else
            free++;
    }

    printf("\nTablas de páginas ocupadas: %d\n", occupied);
    printf("Tablas de páginas libres: %d\n", free);

    fclose(file);
}


void os_start_process(int process_id, char *process_name)
{
    FILE *file = fopen(path, "rb+");
    if (!file)
    {
        printf("Error opening memory file.\n");
        return;
    }

    PCB pcb;
    int found = 0;

    for (int i = 0; i < 32; i++)
    {
        int pcb_offset = i * 256;
        fseek(file, pcb_offset, SEEK_SET);
        fread(&pcb, sizeof(PCB), 1, file);

        if (pcb.state == 0x01 && pcb.process_id == process_id)
        {
            printf("Error: El proceso con ID %d ya existe.\n", process_id);
            fclose(file);
            return;
        }
    }

    for (int i = 0; i < 32; i++)
    {
        int pcb_offset = i * 256;
        fseek(file, pcb_offset, SEEK_SET);
        fread(&pcb, sizeof(PCB), 1, file);

        if (pcb.state == 0x00)
        {
            pcb.state = 0x01;
            pcb.process_id = process_id;
            strncpy(pcb.process_name, process_name, 11);
            pcb.process_name[10] = '\0';

            fseek(file, pcb_offset, SEEK_SET);
            fwrite(&pcb, sizeof(PCB), 1, file);

            printf("Proceso %s (ID: %d) iniciado.\n", process_name, process_id);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("No hay espacio para iniciar un nuevo proceso.\n");
    }

    fclose(file);
}


void liberar_frame_bitmap(FILE *file, uint32_t frame_number)
{
    if (frame_number == 0 || frame_number > 65535)
    {
        printf("Error: Intento de liberar un frame no válido %d\n", frame_number);
        return;
    }
    uint32_t frame_bitmap_offset = (8 * 1024) + 128 + (128 * 1024);

    fseek(file, frame_bitmap_offset + (frame_number / 8), SEEK_SET);

    uint8_t bitmap_byte;
    fread(&bitmap_byte, sizeof(uint8_t), 1, file);

    bitmap_byte &= ~(1 << (frame_number % 8));

    fseek(file, frame_bitmap_offset + (frame_number / 8), SEEK_SET);
    fwrite(&bitmap_byte, sizeof(uint8_t), 1, file);

    printf("Frame %d liberado.\n", frame_number);
}


uint32_t obtener_pfn(FILE *file, uint16_t vpn)
{
    uint8_t first_level_index = vpn >> 6;
    uint8_t second_level_index = vpn & 0x3F;

    uint32_t first_level_address = 8 * 1024 + 128 + (first_level_index * 2);

    printf("Leyendo la tabla de primer nivel en la dirección: 0x%x\n", first_level_address);

    fseek(file, first_level_address, SEEK_SET);
    uint16_t second_level_table_number;
    fread(&second_level_table_number, sizeof(uint16_t), 1, file);

    if (second_level_table_number == 0 || second_level_table_number > 1023)
    {
        printf("Error: tabla de segundo nivel inválida %d en la dirección 0x%x\n", second_level_table_number, first_level_address);
        return -1;
    }

    printf("Tabla de segundo nivel válida: %d\n", second_level_table_number);

    uint32_t second_level_address = 8 * 1024 + 128 + (128 * 1024) + (second_level_table_number * 128);

    fseek(file, second_level_address + (second_level_index * 2), SEEK_SET);
    uint16_t pfn;
    fread(&pfn, sizeof(uint16_t), 1, file);

    if (pfn == 0 || pfn > 65535)
    {
        printf("Error: PFN no válido %d para el VPN %d en la dirección 0x%x\n", pfn, vpn, second_level_address);
        return -1;
    }

    printf("PFN válido: %d para el VPN: %d\n", pfn, vpn);

    return pfn;
}



void liberar_frames(FILE *file, uint32_t virtual_address, uint32_t file_size)
{
    uint32_t remaining_size = file_size;
    uint32_t page_size = 32768;

    if (remaining_size == 0)
    {
        printf("Error: Tamaño de archivo inválido o 0.\n");
        return;
    }

    while (remaining_size > 0)
    {
        uint16_t vpn = (virtual_address >> 15) & 0xFFF;
        uint32_t frame_number = obtener_pfn(file, vpn);

        if (frame_number == 0 || frame_number > 65535)
        {
            printf("Error: frame_number %d no válido\n", frame_number);
            break;
        }

        liberar_frame_bitmap(file, frame_number);

        if (remaining_size > page_size)
        {
            remaining_size -= page_size;
            virtual_address += page_size;
        }
        else
        {
            break;
        }
    }
}


void os_finish_process(int process_id)
{
    FILE *file = fopen(path, "rb+");
    if (!file)
    {
        printf("Error opening memory file.\n");
        return;
    }

    PCB pcb;
    int found = 0;

    for (int i = 0; i < 32; i++)
    {
        int pcb_offset = i * 256;
        fseek(file, pcb_offset, SEEK_SET);
        fread(&pcb, sizeof(PCB), 1, file);

        if (pcb.process_id == process_id && pcb.state == 0x01)
        {
            printf("Terminando proceso con ID: %d\n", process_id);

            for (int j = 0; j < 5; j++)
            {
                if (pcb.file_table[j].valid == 0x01)
                {
                    uint32_t virtual_address = 0;
                    for (int k = 0; k < 4; k++)
                    {
                        virtual_address |= ((uint32_t)pcb.file_table[j].address_bytes[k] << (k * 8));
                    }
                    uint32_t file_size = 0;
                    for (int k = 0; k < 4; k++)
                    {
                        file_size |= ((uint32_t)pcb.file_table[j].size_bytes[k] << (k * 8));
                    }

                    printf("Archivo: %s, Dirección virtual: 0x%x, Tamaño: %d bytes\n",
                           pcb.file_table[j].file_name, virtual_address, file_size);

                    liberar_frames(file, virtual_address, file_size);
                }
            }

            pcb.state = 0x00;

            fseek(file, pcb_offset, SEEK_SET);
            fwrite(&pcb, sizeof(PCB), 1, file);

            found = 1;
            printf("Proceso con ID %d terminado.\n", process_id);
            break;
        }
    }

    if (!found)
    {
        printf("Proceso con ID %d no encontrado.\n", process_id);
    }

    fclose(file);
}


osrmsFile* os_open(int process_id, char *file_name, char mode) {
    FILE *file = fopen(path, "rb+");
    if (!file) {
        printf("Error al abrir el archivo de memoria.\n");
        return NULL;
    }

    int file_exists = os_exists(process_id, file_name);

    if (mode == 'r') {
        if (file_exists == 0) {
            fclose(file);
            printf("El archivo %s no existe.\n", file_name);
            return NULL;
        }

        int process_offset = find_process_offset(file, process_id);
        PCB pcb;
        fseek(file, process_offset, SEEK_SET);
        fread(&pcb, sizeof(PCB), 1, file);

        for (int i = 0; i < 5; i++) {
            if (pcb.file_table[i].valid == 0x01 && strcmp(pcb.file_table[i].file_name, file_name) == 0) {

                printf("Bytes de dirección virtual: %02x %02x %02x %02x\n",
                       pcb.file_table[i].address_bytes[0],
                       pcb.file_table[i].address_bytes[1],
                       pcb.file_table[i].address_bytes[2],
                       pcb.file_table[i].address_bytes[3]);

                osrmsFile *file_desc = malloc(sizeof(osrmsFile));
                file_desc->process_id = process_id;
                strncpy(file_desc->file_name, file_name, 14);

                file_desc->size = 0;
                for (int j = 0; j < 4; j++) {
                    file_desc->size |= ((uint32_t)pcb.file_table[i].size_bytes[j] << (j * 8));
                }

                uint32_t raw_virtual_address = 0;
                for (int j = 0; j < 4; j++) {
                    raw_virtual_address |= ((uint32_t)pcb.file_table[i].address_bytes[j] << (j * 8));
                }

                uint32_t vpn = (raw_virtual_address >> 15) & 0xFFF;
                uint32_t offset = raw_virtual_address & 0x7FFF;

                file_desc->virtual_address = (vpn << 15) | offset;

                printf("Dirección virtual leída: %u (VPN: %u, Offset: %u)\n", file_desc->virtual_address, vpn, offset);
                fclose(file);
                return file_desc;
            }
        }
    } else if (mode == 'w') {
        if (file_exists == 1) {
            printf("Error: El archivo %s ya existe y no puede ser sobrescrito.\n", file_name);
            fclose(file);
            return NULL;
        }

        int process_offset = find_process_offset(file, process_id);
        PCB pcb;
        fseek(file, process_offset, SEEK_SET);
        fread(&pcb, sizeof(PCB), 1, file);

        for (int i = 0; i < 5; i++) {
            if (pcb.file_table[i].valid == 0x00) {
                osrmsFile *file_desc = malloc(sizeof(osrmsFile));
                file_desc->process_id = process_id;
                strncpy(file_desc->file_name, file_name, 14);
                file_desc->size = 0;
                file_desc->virtual_address = 0;

                pcb.file_table[i].valid = 0x01;
                strncpy(pcb.file_table[i].file_name, file_name, 14);

                fseek(file, process_offset, SEEK_SET);
                fwrite(&pcb, sizeof(PCB), 1, file);

                fclose(file);
                return file_desc;
            }
        }

        printf("Error: No hay espacio para crear más archivos en la tabla de archivos.\n");
        fclose(file);
        return NULL;
    }

    printf("Error: Modo inválido.\n");
    fclose(file);
    return NULL;
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

int os_write_file(osrmsFile *file_desc, char *src) {
    FILE *src_file = fopen(src, "rb");
    if (!src_file) {
        printf("Error al abrir el archivo fuente %s.\n", src);
        return -1;
    }
    FILE *mem_file = fopen(path, "rb+");
    if (!mem_file) {
        printf("Error al abrir el archivo de memoria.\n");
        fclose(src_file);
        return -1;
    }

    uint32_t virtual_address = file_desc->virtual_address;
    uint32_t total_bytes_written = 0;
    uint32_t page_size = 32768;
    char buffer[page_size];

    while (1) {
        size_t bytes_read = fread(buffer, 1, page_size, src_file);
        if (bytes_read == 0) {
            break;
        }

        uint16_t vpn = (virtual_address >> 15) & 0xFFF;
        uint16_t offset = virtual_address & 0x7FFF;
        uint32_t frame_number = obtain_pfn(vpn);
        uint32_t frame_pos = obtain_physical_address(frame_number, offset);

        fseek(mem_file, frame_pos, SEEK_SET);
        fwrite(buffer, 1, bytes_read, mem_file);

        total_bytes_written += bytes_read;
        virtual_address += bytes_read;

        if (bytes_read < page_size || total_bytes_written >= file_desc->size) {
            break;
        }
    }

    file_desc->virtual_address = virtual_address;
    file_desc->size = total_bytes_written;

    fclose(src_file);
    fclose(mem_file);

    return total_bytes_written;
}


void os_close(osrmsFile *file_desc)
{
  free(file_desc);
}