#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "command_handlers.h"

// lrexec was based on the video from this class: https://www.youtube.com/watch?v=aqa7YBiGJxU&list=PLVTkEjeczcER2aOMHPIfaHCpTx61iKsoV&index=9
void handle_lrexec_command(char **args, Information *process_list, int *process_count)
{
  pid_t pid = fork();
  if (pid < 0)
  {
    fprintf(stderr, "fork failed");
    exit(1);
  }
  else if (pid == 0)
  {
    execvp(args[0], args);
    fprintf(stderr, "execvp failed");
    exit(1);
  }
  else
  {
    process_list[*process_count].pid = pid;
    strncpy(process_list[*process_count].name, args[0], sizeof(process_list[*process_count].name));
    process_list[*process_count].start_time = time(NULL);
    process_list[*process_count].exit_code = -1;
    (*process_count)++;
    // source: https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-waitpid-wait-specific-child-process-end
    int status;
    waitpid(pid, &status, WNOHANG);
  }
}

void handle_lrlist_command(Information *process_list, int process_count)
{
  printf("PID\t\tName\t\tTime (s)\tExit Code\t\n");
  for (int i = 0; i < process_count; i++)
  {
    time_t current_time = time(NULL);
    double elapsed_time = difftime(current_time, process_list[i].start_time);

    printf("%d\t\t%s\t\t%.0f\t\t%d\t\t\n",
           process_list[i].pid,
           process_list[i].name,
           elapsed_time,
           process_list[i].exit_code);
  }
}

void handle_hello_command()
{
  pid_t pid = fork();
  if (pid < 0)
  {
    fprintf(stderr, "execvp failed");
    exit(1);
  }
  else if (pid == 0)
  {
    printf("Hello World!\n");
    exit(0);
  }
}

void handle_sum_command(char *arg1, char *arg2)
{
  pid_t pid = fork();
  if (pid < 0)
  {
    fprintf(stderr, "execvp failed");
    exit(1);
  }
  else if (pid == 0)
  {
    double num1 = atof(arg1);
    double num2 = atof(arg2);
    double result = num1 + num2;
    printf("Result: %.2f\n", result);
    exit(0);
  }
}

// source: https://www.geeksforgeeks.org/c-program-to-check-whether-a-number-is-prime-or-not/
bool is_prime(int N)
{
  if (N <= 1)
  {
    return false;
  }
  for (int i = 2; i < N; i++)
  {
    if (N % i == 0)
    {
      return false;
    }
  }
  return true;
}

void handle_is_prime_command(char *arg)
{
  pid_t pid = fork();
  if (pid < 0)
  {
    fprintf(stderr, "execvp failed");
    exit(1);
  }
  else if (pid == 0)
  {
    // atoi source: https://www.aprendeaprogramar.com/referencia/view.php?f=atoi&leng=C
    int num = atoi(arg);
    if (is_prime(num))
    {
      printf("%d is a prime number.\n", num);
    }
    else
    {
      printf("%d is not a prime number.\n", num);
    }
    exit(0);
  }
}
