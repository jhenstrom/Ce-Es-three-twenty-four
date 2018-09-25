#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  FILE *f;
  int sleep_time = 1;

  char *sleep_env_var = getenv("SLOWCAT_SLEEP_TIME");
  if (sleep_env_var != NULL)
  {
    sleep_time = atoi(sleep_env_var);
  }
  if (argc > 1)
  {
    f = fopen(argv[1], "r");
  }
  else
  {
    f = stdin;
  }
  printf("%d\n", getpid());
  char line[50];
  while(fgets(line, 50, f))
  {
    printf("%s", line);
    sleep(sleep_time);
  }
  return 0;
}
