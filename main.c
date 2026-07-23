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

	Getcwd(cwd, sizeof(cwd));

	p("%s $>", cwd);
	Getline(&line, &size, stdin);

	return(line);
}


void history_save(char *line)
{
	FILE *file;

	file = fopen("command_history.txt", "a");

	if (file == NULL)
	{
		perror("fopen");
	}
	else
	{
		fprintf(file, "%s", line);
		fflush(file);
	}
	fclose(file);
}


int apply_redirections(command *argv)
{
	if (argv->input_file[0] != '\0')
	{
	 	int if_desc = open(argv->input_file, O_RDONLY);
		if (if_desc < 0)
		{
			perror("open");
			return(1);
		}
		if ((dup2(if_desc, STDIN_FILENO)) == -1)
		{
			perror("dup2");
			close(if_desc);
			return(1);
		}
		close(if_desc);
	}
	if (argv->output_file[0] != '\0')
	{
 		int of_desc = open(argv->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (of_desc < 0)
		{
			perror("open");
			return(1);
		}
		if ((dup2(of_desc, STDOUT_FILENO)) == -1)
		{
			perror("dup2");
			close(of_desc);
			return(1);
		}
		close(of_desc);
	}
	return(0);
}


void general_exec(command *argv)
{
	pid_t pid = Fork();

    if (pid == 0)
    {
    	apply_redirections(argv);
		Execvp(argv->tokens[0], argv->tokens);
	}
    else
    	Wait(&status);
}


void builtin_exec(command *argv)
{
	int i = 0;
	const char *curr;
	while ((curr = global_builtin[i].builtin_name))
    {
        if (!strcmp(curr, argv->tokens[0]))
        {
			int org_stdout_desc = dup(STDOUT_FILENO);
			int org_stdin_desc = dup(STDIN_FILENO);
			apply_redirections(argv);
			fflush(stdout);

            status = global_builtin[i].foo(argv);

           	dup2(org_stdout_desc, STDOUT_FILENO);
			dup2(org_stdin_desc, STDIN_FILENO);
			close(org_stdout_desc);
			close(org_stdin_desc);
            return;
        }
        ++i;
    }
    general_exec(argv);
}


void free_command(command *argv)
{
	for (int i = 0; argv->tokens[i] != NULL; i++)
		free(argv->tokens[i]);
	free(argv->tokens);
}


int main()
{
	char *line;
	command argv;

	// REPL
	// LOOP WHILE READ LINE
	while((line = read_line()))
	{
		//SAVE TO HISTORY
		history_save(line);

		// EVALUATE
		argv = parsing(line);
		if (argv.error != 0)
		{
			free(line);
			free_command(&argv);
			continue;
		}

		// PRINT
		builtin_exec(&argv);

		free(line);
		free_command(&argv);
	}
	return(EXIT_SUCCESS);
}
