#include "lib.h"

int cmd_cd(command *current)
{
	const char *path = current->tokens[1];

	if (!path)
	{
		path = Getenv("HOME");
	}
	return (Chdir(path));
}


extern char **environ;

int cmd_env(command *current)
{
	(void)current;

    for (char **curr = environ; *curr; curr++)
    {
        puts(*curr);
    }
    return (EXIT_SUCCESS);
}


int cmd_echo(command *current)
{
	for (int i = 1; current->tokens[i] != NULL; i++)
	{
		p("%s", current->tokens[i]);
		if (current->tokens[i+1] != NULL)
		{
			p(" ");
		}
	}
	p("\n");
	return (0);
}


int cmd_history(command *current)
{
    (void)current;
    FILE *file;
    char line[2048];

	file = fopen("command_history.txt", "r");

    if (file == NULL)
    {
		perror("fopen");
		return(1);
	}

	for (int i = 0; fgets(line, sizeof line, file) != NULL; i++)
	{
		p("%d %s", i + 1, line);
	}
	fclose(file);
	return(0);
}


int cmd_exit(command *current)
{
	(void)current;

	exit(EXIT_SUCCESS);
}
