#include "lib.h"


int parsing(char *line, command *cmd1, command *cmd2)
{
	size_t bufsize = BUFSIZ;
	unsigned int tok_pos = 0;
	unsigned int n = 0;
	char token[256];
	int has_pipe = 0;

	command *current = cmd1;

	cmd1->tokens = Malloc(bufsize * sizeof *cmd1->tokens);
	cmd1->input_file[0] = '\0';
	cmd1->output_file[0] = '\0';
	cmd1->append_file[0] = '\0';
	cmd1->error = 0;

	cmd2->tokens = Malloc(bufsize * sizeof *cmd2->tokens);
	cmd2->input_file[0] = '\0';
	cmd2->output_file[0] = '\0';
	cmd2->append_file[0] = '\0';
	cmd2->error = 0;
	cmd2->tokens[0] = NULL; // To avoid a free() crush if pipe wasn't found

	for (int i = 0; line[i] != '\0'; i++)
	{
		switch (line[i])
		{
			case '|':
				i++;
				current->tokens[n] = NULL;
				if (pipe_state(&has_pipe, &n, &tok_pos, &current, cmd2) != 0)
				{
					current->tokens[n] = NULL;
					current->error = 1;
					return(1);
				}
				break;

			case '\\':
				if (escape_ch_state(line, token, &tok_pos, &i) != 0)
				{
					current->tokens[n] = NULL;
					current->error = 1;
					return(1);
				}
				break;

			case '>':
				if (output_redirector_state(line, current->output_file, current->append_file, &i) != 0)
				{
					current->tokens[n] = NULL;
					current->error = 1;
					return(1);
				}
				break;

			case '<':
				if (input_redirector_state(line, current->input_file, &i) != 0)
				{
					current->tokens[n] = NULL;
					current->error = 1;
					return(1);
				}
				break;

			case '"':
				if (double_quotes_state(line, token, &tok_pos, &i) != 0)
				{
					current->tokens[n] = NULL;
					current->error = 1;
					return(1);
				}
				break;

			case '\'':
				if (single_quotes_state(line, token, &tok_pos, &i) != 0)
				{
					current->tokens[n] = NULL;
					current->error = 1;
					return(1);
				}
				break;

			case '$':
				if (var_expansion_state(line, token, &tok_pos, &i) != 0)
				{
					current->tokens[n] = NULL;
					current->error = 1;
					return(1);
				}
				break;

			default:
				if (isspace((unsigned char)line[i]))
				{
					while (isspace(line[i+1]))
					{
						i++;
					}
					token[tok_pos] = '\0';

					current->tokens[n] = Malloc(strlen(token) + 1);
					strcpy(current->tokens[n], token);
					n++;

					if (n >= bufsize)
        			{
            			bufsize *= 2;
            			current->tokens = Realloc(current->tokens, bufsize * sizeof(*current->tokens));
        			}
        			memset(token, 0, sizeof(token));
        			tok_pos = 0;
        		}
        		else
        		{
        			token[tok_pos] = line[i];
        			tok_pos++;
        		}
		}
	}
	if (tok_pos > 0)
	{
		token[tok_pos] = '\0';

		current->tokens[n] = Malloc(strlen(token) + 1);
		strcpy(current->tokens[n], token);
		n++;
		if (n >= bufsize)
    	{
    		bufsize *= 2;
        	current->tokens = Realloc(current->tokens, bufsize * sizeof(*current->tokens));
    	}
	}
	current->tokens[n] = NULL;
	return has_pipe;
}


int pipe_state(int *has_pipe, unsigned int *n, unsigned int *tok_pos, command **current, command *cmd2)
{
	if (*has_pipe)
	{
		fp(stderr, "Parser: can't handle second pipe(|)\n");
		return(1);
	}
	*has_pipe = 1;
	*n = 0;
	*tok_pos = 0;
	*current = cmd2;
	return(0);
}


int escape_ch_state(char *line, char *token, unsigned int *tok_pos, int *i)
{
	(*i)++;
	if (line[*i] == '\'')
	{
		fp(stderr, "Parser: can't handle escape character with (')\n");
		return(1);
	}
	else
	{
		token[*tok_pos] = line[*i];
		(*tok_pos)++;
		(*i)++;
	}
	(*i)--;
	return(0);
}


int append_redirector_state(char *line, char *append_file, int *i)
{
	unsigned int af_pos = 0;
	while (isspace(line[*i]))
	{
		(*i)++;
	}
	while (line[*i] != '\0' && !isspace(line[*i]))
	{
		append_file[af_pos] = line[*i];
		af_pos++;
		(*i)++;
	}
	append_file[af_pos] = '\0';
	if (append_file[0] == '\0')
		return(1);
	return(0);
}


int output_redirector_state(char *line, char *output_file, char *append_file, int *i)
{
	if (line[*i] == '>')
	{
		(*i)++;
		if (line[*i] == '>')
		{
			(*i)++;
			append_redirector_state(line, append_file, i);
		}
	}

	if (append_file[0] != '\0')
		return(0);

	if (output_file[0] != '\0')
	{
		fp(stderr, "Parser: can't handle second output redirector(>)\n");
		return(1);
	}
	else
		(*i)++;

	unsigned int of_pos = 0;
	while (isspace(line[*i]))
	{
		(*i)++;
	}
	while (line[*i] != '\0' && !isspace(line[*i]))
	{
		output_file[of_pos] = line[*i];
		of_pos++;
		(*i)++;
	}
	output_file[of_pos] = '\0';
	if (output_file[0] == '\0')
		return(1);
	return(0);
}


int input_redirector_state(char *line, char *input_file, int *i)
{
	if (input_file[0] != '\0')
	{
		fp(stderr, "Parser: can't handle second input redirector(<)\n");
		return(1);
	}
	else
		(*i)++;

	unsigned int if_pos = 0;
	while (isspace(line[*i]))
	{
		(*i)++;
	}
	while (line[*i] != '\0' && !isspace(line[*i]))
	{
		input_file[if_pos] = line[*i];
		if_pos++;
		(*i)++;
	}
	input_file[if_pos] = '\0';
	if (input_file[0] == '\0')
		return(1);
	return(0);
}


int double_quotes_state(char *line, char *token, unsigned int *tok_pos, int *i)
{
	(*i)++;
	while (line[*i] != '"')
	{
		if (line[*i] != '$')
		{
			if (line[*i] == '\0')
			{
				fp(stderr, "Parser: didn't find closing quote\n");
				return(1);
			}
			else
			{
				if (line[*i] == '\\')
				{
					if (escape_ch_state(line, token, tok_pos, i) != 0)
						return(1);
					(*i)++;
				}
				token[*tok_pos] = line[*i];
				(*tok_pos)++;
				(*i)++;
			}
		}
		else
		{
			if (var_expansion_state(line, token, tok_pos, i) != 0)
				return(1);
			(*i)++;
		}
	}
	return(0);
}


int single_quotes_state(char *line, char *token, unsigned int *tok_pos, int *i)
{
	(*i)++;
	while (line[*i] != '\'')
	{
		if (line[*i] == '\0')
		{
			fp(stderr, "Parser: didn't find closing quote\n");
			return(1);
		}
		else
		{
			token[*tok_pos] = line[*i];
			(*tok_pos)++;
			(*i)++;
		}
	}
	return(0);
}


int var_expansion_state(char *line, char *token, unsigned int *tok_pos, int *i)
{
	if (line[*i] == '$')
	{
		char var_buffer[256];
		unsigned int var_pos = 0;
		var_buffer[var_pos] = line[*i];
		var_pos++;
		(*i)++;
		while (line[*i] != '\0' && !isspace((unsigned char)line[*i]) && line[*i] != '"')
		{
			if (isupper(line[*i]) || isdigit(line[*i]) || line[*i] == '_')
			{
				var_buffer[var_pos] = line[*i];
				var_pos++;
				(*i)++;
			}
			else
			{
				fp(stderr, "Parser: invalid character in an env var\n");
				return(1);
			}
		}
		(*i)--;
		var_buffer[var_pos] = '\0';
		const char *dollar_var = Getenv(var_buffer + 1);
		for (int d_pos = 0; dollar_var[d_pos] != '\0'; d_pos++)
		{
			token[*tok_pos] = dollar_var[d_pos];
			(*tok_pos)++;
		}
	}
	return(0);
}
