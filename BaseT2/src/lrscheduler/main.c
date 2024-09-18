#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];

	// TODO: change to argv[3]
	int16_t quantum = (int16_t)atoi(argv[2]);
	InputFile *input_file = read_file(file_name);

	Queue *high_queue = create_queue(2 * quantum);
	Queue *low_queue = create_queue(quantum);
	for (int i = 0; i < input_file->len; ++i)
	{
		Process *process = process_create(
				input_file->lines[i][0],
				atoi(input_file->lines[i][1]),
				(ProcessState)READY,
				atoi(input_file->lines[i][2]),
				atoi(input_file->lines[i][3]),
				atoi(input_file->lines[i][4]),
				atoi(input_file->lines[i][5]));

		enqueue(high_queue, process);
	}

	input_file_destroy(input_file);
}