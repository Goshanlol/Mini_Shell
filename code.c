#include "lib.h"

int status = 0;

char **g_history;
int g_history_size = 0;


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


void history_inc(char *line)
{
	char *copy = Malloc(strlen(line) + 1);
	strcpy(copy, line);

	g_history[g_history_size] = copy;
	g_history_size++;
}


char **parsing(char *line)
{
    char **tokens;
    unsigned int position;
    size_t bufsize = BUFSIZ;

    tokens = Malloc(bufsize * sizeof *tokens);
    position = 0;

    for (char *token = strtok(line, DEL); token; token = strtok(NULL, DEL))
    {
        tokens[position++] = token;
        if (position >= bufsize)
        {
            bufsize *= 2;
            tokens = Realloc(tokens, bufsize * sizeof(*tokens));
        }
    }
    tokens[position] = NULL;

	for (int i = 1; tokens[i] != NULL; i++)
	{
		const char *dollar_var = strchr(tokens[i], '$');

		if (dollar_var != NULL)
		{
			tokens[i] = getenv(dollar_var + 1);
		}
	}
	return (tokens);
}


void execution(char **argv)
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


void cmd_exec(char **argv)
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
    execution(argv);
}


int main()
{
	char *line;
	char **argv;

	g_history = Malloc(BUFSIZ * sizeof(*g_history));

	// REPL
	// LOOP WHILE READ LINE
	while((line = read_line()))
	{
		//SAVE TO HISTORY
		history_inc(line);

		// EVALUATE
		argv = parsing(line);

		// PRINT
		cmd_exec(argv);

		free(line);
		free(argv);
	}
	return(EXIT_SUCCESS);
}
