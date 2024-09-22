#include "process.h"
#include <stdlib.h>

Process *process_create(char *name, int pid, int32_t initialTime, int burstTime, int numBursts, int ioWaitTime, int deadline)
{
  Process *process = (Process *)malloc(sizeof(Process));
  process->name = name;
  process->pid = pid;
  process->initialTime = initialTime;
  process->burstTime = burstTime;
  process->numBursts = numBursts;
  process->ioWaitTime = ioWaitTime;
  process->deadline = deadline;
  process->state = READY;
  return process;
}