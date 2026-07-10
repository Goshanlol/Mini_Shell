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


char *Getcwd(char *buf, size_t size)
{
	char *ptr = getcwd(buf, size);

	if (ptr == NULL)
	{
		perror("getcwd");
		exit(EXIT_FAILURE);
	}
	return(ptr);
}


char *Getenv(const char *name)
{
	char *ptr = getenv(name);

	if (ptr == NULL)
	{
		return("");
	}
	return(ptr);
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
		fp(stderr, "Execvp: invalid arguments\n");
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
		fp(stderr, "Wait: status argument required\n");
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
    else
    {
    	perror("chdir");
    	return(1);
    }
}
