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

	p("♣️ %s ♣️ >", cwd);
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


int apply_redirections(command *current)
{
	if (current->input_file[0] != '\0')
	{
	 	int if_desc = open(current->input_file, O_RDONLY);
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

	if (current->output_file[0] != '\0')
	{
 		int of_desc = open(current->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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

	if (current->append_file[0] != '\0')
	{
 		int af_desc = open(current->append_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
		if (af_desc < 0)
		{
			perror("open");
			return(1);
		}
		if ((dup2(af_desc, STDOUT_FILENO)) == -1)
		{
			perror("dup2");
			close(af_desc);
			return(1);
		}
		close(af_desc);
	}
	return(0);
}


void apply_heredoc_redirection(command *current)
{
	FILE *heredoc_file = tmpfile();
	size_t size = 0;
	char *line = NULL;

	while (1)
	{
   		p("♣️ >");
    	if (Getline(&line, &size, stdin) == -1)
       		break;
    	if (strcmp(line, current->heredoc_del) == 0)
    	{
    		rewind(heredoc_file);
    		int hf_fd = fileno(heredoc_file);
			pid_t pid = Fork();
			if (pid == 0)
			{
				dup2(hf_fd, STDIN_FILENO);
				int i = 0;
				const char *curr;
				while ((curr = global_builtin[i].builtin_name))
    			{
   	 				if (!strcmp(curr, current->tokens[0]))
   	 				{
						apply_redirections(current);
	            		status = global_builtin[i].foo(current);
	          		  	exit(EXIT_SUCCESS);
        			}
        			++i;
    			}
				Execvp(current->tokens[0], current->tokens);
			}
			waitpid(pid, &status, 0);
			fclose(heredoc_file);
   		    break;
    	}
	    fputs(line, heredoc_file);
	}
}


void pipe_exec(command *cmd1, command *cmd2)
{
	int fd[2];

	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid_t pid1 = Fork();
	if (pid1 == 0)
	{
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2");
			close(fd[1]);
			exit(EXIT_FAILURE);
		}
		close(fd[1]);

		// builtin_exec
		int i = 0;
		const char *curr;
		while ((curr = global_builtin[i].builtin_name))
    	{
   	 		if (!strcmp(curr, cmd1->tokens[0]))
   	 		{
				apply_redirections(cmd1);
	            status = global_builtin[i].foo(cmd1);
	            exit(EXIT_SUCCESS);
        	}
        	++i;
    	}
    	// general_exec
		apply_redirections(cmd1);
		Execvp(cmd1->tokens[0], cmd1->tokens);
	}

	pid_t pid2 = Fork();
	if (pid2 == 0)
	{
		close(fd[1]);
		if (dup2(fd[0], STDIN_FILENO) == -1)
		{
			perror("dup2");
			close(fd[0]);
			exit(EXIT_FAILURE);
		}
		close(fd[0]);

		// builtin_exec
		int i = 0;
		const char *curr;
		while ((curr = global_builtin[i].builtin_name))
    	{
   	 		if (!strcmp(curr, cmd2->tokens[0]))
   	 		{
				apply_redirections(cmd2);
	            status = global_builtin[i].foo(cmd2);
	            exit(EXIT_SUCCESS);
        	}
        	++i;
    	}
    	// general_exec
		apply_redirections(cmd2);
		Execvp(cmd2->tokens[0], cmd2->tokens);
	}

	close(fd[1]);
	close(fd[0]);

	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
}


void general_exec(command *current)
{
	pid_t pid = Fork();

    if (pid == 0)
    {
   		apply_redirections(current);
   		Execvp(current->tokens[0], current->tokens);
	}
    else
    	waitpid(pid, &status, 0);
}


void builtin_exec(command *current)
{
	if (current->heredoc_del[0] == '\0')
	{
		int i = 0;
		const char *curr;
		while ((curr = global_builtin[i].builtin_name))
	    {
    	    if (!strcmp(curr, current->tokens[0]))
        	{
				int org_stdout_desc = dup(STDOUT_FILENO);
				int org_stdin_desc = dup(STDIN_FILENO);
				apply_redirections(current);
				fflush(stdout);

	            status = global_builtin[i].foo(current);

    	       	dup2(org_stdout_desc, STDOUT_FILENO);
				dup2(org_stdin_desc, STDIN_FILENO);
				close(org_stdout_desc);
				close(org_stdin_desc);
            	return;
        	}
        	++i;
    	}
    	general_exec(current);
    }
    else
    {
		apply_heredoc_redirection(current);
	}
}


void free_command(command *current)
{
	for (int i = 0; current->tokens[i] != NULL; i++)
		free(current->tokens[i]);
	free(current->tokens);
}


int main()
{
	char *line;
	command cmd1;
	command cmd2;

	// REPL
	// LOOP WHILE READ LINE
	while((line = read_line()))
	{
		// SAVE TO HISTORY
		history_save(line);

		// EVALUATE
		int result = parsing(line, &cmd1, &cmd2);
		if (cmd1.error != 0 || cmd2.error != 0)
		{
			free(line);
    		free_command(&cmd1);
	    	free_command(&cmd2);
    		continue;
    	}
		if (result == 0)
			builtin_exec(&cmd1);
		else
			pipe_exec(&cmd1, &cmd2);

		free(line);
		free_command(&cmd1);
		free_command(&cmd2);
	}
	return(EXIT_SUCCESS);
}
