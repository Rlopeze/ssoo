#include "../osrms_API/osrms_API.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{

    // montar la memoria
    os_mount((char *)argv[1]);
    os_ls_processes();
    printf("os_exists(91, \"knowledg.jpg\"): %d\n", os_exists(91, "knowledg.jpg"));
    // os_ls_files(91);

    return 0;
}