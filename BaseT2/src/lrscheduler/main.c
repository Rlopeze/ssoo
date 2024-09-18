#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];

	// TODO: change to argv[3], argv[2] will be the output.csv
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
	while (is_empty(high_queue) == false || is_empty(low_queue) == false)
	{
		// TODO: Implement the logic of the scheduler
		// 1. Check if there are processes in the high queue
		// 2. Get priority to execute a process from the high queue
		// 3. If there are processes in the high queue, dequeue one and execute it
		// 4. If there are no processes in the high queue, get priority to execute a process from the low queue
	}

	input_file_destroy(input_file);
}