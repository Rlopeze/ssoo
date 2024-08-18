#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h> // Add this line

#include "command_handlers.h"

// Función para verificar si un número es primo (repetida aquí para la modularización)
bool is_prime(int num)
{
  if (num <= 1)
    return false;
  if (num == 2 || num == 3)
    return true;
  if (num % 2 == 0 || num % 3 == 0)
    return false;
  for (int i = 5; i * i <= num; i += 6)
  {
    if (num % i == 0 || num % (i + 2) == 0)
      return false;
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
    // Proceso hijo
    printf("Hello World!\n");
    exit(0); // Terminar el proceso hijo
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
    // Proceso hijo
    double num1 = atof(arg1);
    double num2 = atof(arg2);
    double result = num1 + num2;
    printf("Result: %.2f\n", result);
    exit(0); // Terminar el proceso hijo
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
    // Proceso hijo
    int num = atoi(arg);
    if (is_prime(num))
    {
      printf("%d is a prime number.\n", num);
    }
    else
    {
      printf("%d is not a prime number.\n", num);
    }
    exit(0); // Terminar el proceso hijo
  }
}
