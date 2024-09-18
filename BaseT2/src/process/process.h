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
  ProcessState state;
  int burstTime;
  int numBursts;
  int ioWaitTime;
  int deadline;
} Process;

Process *process_create(char *name, int pid, ProcessState state, int burstTime, int numBursts, int ioWaitTime, int deadline);