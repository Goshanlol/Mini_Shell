#ifndef lib_h

#define lib_h

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

extern char **g_history;
extern int g_history_size;

#define DEL "\n\t\v\f\r "
#define p(...) printf(__VA_ARGS__)

typedef struct struct_builtin
{
    const char *builtin_name;
    int (*foo)(char**);
} type_builtin;

char **parsing(char *);

ssize_t Getline(char **, size_t *, FILE *);
void *Malloc(size_t);
void *Realloc(void *, size_t);
pid_t Fork(void);
void Execvp(const char *file, char *const argv[]);
pid_t Wait(int *);
int Chdir(const char *path);

int cmd_cd(char **);
int cmd_env(char **);
int cmd_echo(char **);
int cmd_history(char **);
int cmd_exit(char **);

#endif
