#pragma once
#include <stdint.h>

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
  int actualBurstTime;
  int numBursts;
  int32_t ioWaitTime;
  int32_t ioWaitTimeLeft;
  int32_t deadline;
  ProcessState state;
  int last_cpu_tick;
  int interrupciones;
  int response_time;
  int waiting_time;
  int suma_deadline;
  int quantum;

  int has_enter_cpu_first_time;
  int has_left_cpu_first_time;
} Process;

Process *process_create(char *name, int pid, int32_t initialTime, int burstTime, int numBursts, int32_t ioWaitTime, int32_t deadline, int32_t quantum);