#include "lib.h"

int status = 0;

type_builtin global_builtin[] =
{
	{.builtin_name="cd", .foo=cmd_cd},
	{.builtin_name="env", .foo=cmd_env},
	{.builtin_name="echo", .foo=cmd_echo},
	{.builtin_name="history", .foo=cmd_history},
	{.builtin_name="exit", .foo=cmd_exit},
	{.builtin_name=NULL},
};


char *read_line(void)
{
	char *line = NULL;
	size_t size = 0;
	char cwd[4096];

	getcwd(cwd, sizeof(cwd));

	p("%s $>", cwd);
	Getline(&line, &size, stdin);

	return(line);
}


char *history_save(char *line)
{
	FILE *file;

	file = fopen("command_history.txt", "a");

	if (file == NULL)
	{
		perror("Could not open or create the file\n");
	}
	else
	{
		fprintf(file, "%s", line);
		fflush(file);
	}
	fclose(file);
}


void general_exec(char **argv)
{
	pid_t pid = Fork();

    if (pid == 0)
	{
		Execvp(argv[0], argv);
    }
    else
    {
    	Wait(&status);
    }
}


void builtin_exec(char **argv)
{
	int i;
	const char *curr;

	i = 0;
	while ((curr = global_builtin[i].builtin_name))
    {
        if (!strcmp(curr, argv[0]))
        {
            status = global_builtin[i].foo(argv);
            return;
        }
        ++i;
    }
    general_exec(argv);
}


int main()
{
	char *line;
	char **argv;

	// REPL
	// LOOP WHILE READ LINE
	while((line = read_line()))
	{
		//SAVE TO HISTORY
		history_save(line);

		// EVALUATE
		argv = parsing(line);

		// PRINT
		builtin_exec(argv);

		free(line);
		free(argv);
	}
	return(EXIT_SUCCESS);
}
