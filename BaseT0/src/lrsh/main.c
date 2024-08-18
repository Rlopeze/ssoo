#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "../input_manager/manager.h"
#include "command_handlers.h"

void handle_sigchld(int sig)
{
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
}

int main()
{
  struct sigaction sa;
  sa.sa_handler = &handle_sigchld;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, 0);

  while (1)
  {
    printf("shell> ");

    char **tokens = read_user_input();

    if (tokens[0] == NULL)
    {
      free_user_input(tokens);
      continue;
    }

    if (strcmp(tokens[0], "hello") == 0)
    {
      handle_hello_command();
    }
    else if (strcmp(tokens[0], "sum") == 0 && tokens[1] != NULL && tokens[2] != NULL)
    {
      handle_sum_command(tokens[1], tokens[2]);
    }
    else if (strcmp(tokens[0], "is_prime") == 0 && tokens[1] != NULL)
    {
      handle_is_prime_command(tokens[1]);
    }
    else if (strcmp(tokens[0], "exit") == 0)
    {
      free_user_input(tokens);
      break;
    }
    else
    {
      printf("Comando no reconocido o formato incorrecto.\n");
    }

    free_user_input(tokens);
  }

  return 0;
}
