#include "lib.h"

ssize_t Getline(char **line, size_t *size, FILE *stream)
{
	ssize_t r = getline(line, size, stream);

	if (r == -1)
	{
    	if (feof(stream))
    	{
        	exit(0);
    	}
    	else
    	{
        	perror("getline");
        	exit(1);
    	}
	}
	return (r);
}


void *Malloc(size_t size)
{
	void *ptr = malloc(size);

	if (!ptr)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	return (ptr);
}


void *Realloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);

	if (!ptr)
	{
		perror("realloc");
		exit(EXIT_FAILURE);
	}
	return (ptr);
}


pid_t Fork(void)
{
	pid_t pid = fork();

	if (pid == -1)
	{
		perror("fork");
		exit(EX_OSERR);
	}
	return (pid);
}


void Execvp(const char *file, char *const argv[])
{
	if (!file || !argv || !argv[0])
	{
		fprintf(stderr, "Execvp: invalid arguments\n");
		exit(EXIT_FAILURE);
	}
	if (execvp(file, argv) == -1)
	{
		perror(file);
		exit(EX_UNAVAILABLE);
	}
}


pid_t Wait(int *status)
{
	pid_t pid;

	if (!status)
	{
		fprintf(stderr, "Wait: status argument required\n");
		return (-1);
	}

	pid = wait(status);

	if (pid == -1)
	{
		perror("wait");
	}
	return (pid);
}


int Chdir(const char *path)
{
    if (chdir(path) == 0)
        return 0;

    if (errno == ENOENT)
        p("No such file or directory\n");
    else if (errno == EACCES)
        p("Permission denied\n");
    else if (errno == ELOOP)
        p("Too many symbolic links\n");
    else if (errno == ENOTDIR)
        p("Not a directory\n");
    else
        p("chdir failed\n");

    return 1;
}
