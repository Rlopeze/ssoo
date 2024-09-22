#pragma once

typedef enum
{
  RUNNING,
  READY,
  WAITING,
  FINISHED
} ProcessState;

typedef struct
{
  char *name;
  int pid;
  int32_t initialTime;
  int burstTime;
  int numBursts;
  int32_t ioWaitTime;
  int32_t deadline;
  ProcessState state;
} Process;

Process *process_create(char *name, int pid, int32_t initialTime, int burstTime, int numBursts, int ioWaitTime, int deadline);