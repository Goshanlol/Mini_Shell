#include "lib.h"

int cmd_cd(char **argv)
{
	const char *path = argv[1];

	if (!path)
	{
		path = Getenv("HOME");
	}
	return (Chdir(path));
}


extern char **environ;

int cmd_env(char **argv)
{
	(void)argv;

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
    FILE *file;
    char line[2048];

	file = fopen("command_history.txt", "r");

    if (file == NULL)
    {
		perror("fopen");
	}

	for (int i = 0; fgets(line, sizeof line, file) != NULL; i++)
	{
		p("%d %s", i + 1, line);
	}
	fclose(file);
	return(0);
}


int cmd_exit(char **argv)
{
	(void)argv;

	exit(EXIT_SUCCESS);
}
