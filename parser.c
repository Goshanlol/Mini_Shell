#include "lib.h"


command parsing(char *line)
{
	size_t bufsize = BUFSIZ;
	unsigned int tok_pos = 0;
	unsigned int n = 0;
	char token[256];

	command args;
	args.tokens = Malloc(bufsize * sizeof *args.tokens);
	args.input_file[0] = '\0';
	args.error = 0;

	for (int i = 0; line[i] != '\0'; i++)
	{
		switch (line[i])
		{
			case '<':
				if (left_redirector_state(line, args.input_file, &i) != 0)
				{
					args.error = 1;
					return args;
				}
				break;

			case '"':
				if (double_quotes_state(line, token, &tok_pos, &i) != 0)
				{
					args.error = 1;
					return args;
				}
				break;

			case '\'':
				if (single_quotes_state(line, token, &tok_pos, &i) != 0)
				{
					args.error = 1;
					return args;
				}
				break;

			case '$':
				if (var_expansion_state(line, token, &tok_pos, &i) != 0)
				{
					args.error = 1;
					return args;
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

					args.tokens[n] = Malloc(strlen(token) + 1);
					strcpy(args.tokens[n], token);
					n++;

					if (n >= bufsize)
        			{
            			bufsize *= 2;
            			args.tokens = Realloc(args.tokens, bufsize * sizeof(*args.tokens));
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

		args.tokens[n] = Malloc(strlen(token) + 1);
		strcpy(args.tokens[n], token);
		n++;
		if (n >= bufsize)
    	{
    		bufsize *= 2;
        	args.tokens = Realloc(args.tokens, bufsize * sizeof(*args.tokens));
    	}
	}
	args.tokens[n] = NULL;
	return args;
}


int left_redirector_state(char *line, char *input_file, int *i)
{
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
