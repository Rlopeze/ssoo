#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

#include "command_handlers.h"

void handle_lrexec_command(char **args)
{
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("fork failed");
    exit(1);
  }
  else if (pid == 0)
  {
    execvp(args[0], args);
    perror("exec failed");
    exit(1);
  }
  else
  {
    // source: https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-waitpid-wait-specific-child-process-end
    int status;
    waitpid(pid, &status, WNOHANG);
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

void handle_hello_command()
{
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("fork failed");
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
  if (pid == -1)
  {
    perror("fork failed");
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

void handle_is_prime_command(char *arg)
{
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("fork failed");
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
