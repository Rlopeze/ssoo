#include "../osrms_API/osrms_API.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{

    // montar la memoria
    os_mount((char *)argv[1]);

    // Probar las funciones (descomentar los bloques respectivos)


    // os_ls_processes(); Listamos los procesos
    // os_ls_processes();

    // os_exists(91, "knowledg.jpg"); Verificamos que exista el archivo
    os_exists(91, "knowledg.jpg");

    // os_ls_files(91); Listamos los archivos del proceso 91
    os_ls_files(91);

    // os_frame_bitmap(); Mostramos el bitmap de frames y el conteo de frames ocupados y libres
    os_frame_bitmap();

    // os_tp_bitmap(); Mostramos el bitmap de tablas de páginas y el conteo de tablas de páginas ocupadas y libres
    os_tp_bitmap();

    // os_start_process(123, "Process123"); Iniciamos un proceso, y verificamos que se haya iniciado
    os_start_process(123, "Process123");
    os_ls_processes();

    // os_finish_process(123); Finalizamos un proceso, y verificamos que se haya finalizado
    os_finish_process(123);
    os_ls_processes();


    // Abrimos un archivo para lectura y leemos su contenido
    // Casos de error (para 'r' archivo no existe, para 'w' archivo ya existe)
    osrmsFile *non_existent_file = os_open(91, "new.jpg", 'r');
    osrmsFile *existent_file = os_open(91, "new.jpg", 'w');

    // Casos correctos
    osrmsFile *file_for_reading = os_open(91, "knowledg.jpg", 'r');
    osrmsFile *file_for_writing = os_open(91, "new.jpg", 'w');

    // Leemos el archivo y guardamos su contenido en el directorio actual
    os_read_file(file_for_reading, "./knowledg_copia.jpg");

    // Escribimos en el archivo
    os_write_file(file_for_writing, "./knowledg.jpg");



    return 0;
}