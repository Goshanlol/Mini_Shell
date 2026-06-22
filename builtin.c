#include "lib.h"

int cmd_cd(char **argv)
{
	const char *path = argv[1];

	if (!path)
	{
		path = getenv("HOME");
	}
	return (Chdir(path));
}


extern char **environ;

int cmd_env(char **argv)
{
    for (char **current = environ; *current; current++)
    {
        puts(*current);
    }
    return (EXIT_SUCCESS);
}


int cmd_echo(char **argv)
{
	for (int i = 1; argv[i] != NULL; i++)
	{
		p("%s", argv[i]);
		if (argv[i+1] != NULL)
		{
			p(" ");
		}
	}
	p("\n");
	return (0);
}


int cmd_history(char **argv)
{
    (void)argv;

    for (int i = 0; i < g_history_size; i++)
    {
        p("%d %s", i + 1, g_history[i]);
    }
    return (0);
}


int cmd_exit(char **argv)
{
	(void)argv;

	exit(EXIT_SUCCESS);
}
