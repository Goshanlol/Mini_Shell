#include "lib.h"


char **parsing(char *line)
{
	char **tokens;
	size_t bufsize = BUFSIZ;
	unsigned int tok_pos = 0;
	unsigned int n = 0;
	char token[256];

	tokens = Malloc(bufsize * sizeof *tokens);

	for (int i = 0; line[i] != '\0'; i++)
	{
		switch (line[i])
		{
			case '"':
				i++;
				while (line[i] != '"')
				{
					if (line[i] != '$')
					{
						if (line[i] == '\0')
						{
							fprintf(stderr, "Parser: didn't find closing quote\n");
							return(NULL);
						}
						else
						{
							token[tok_pos] = line[i];
							tok_pos++;
							i++;
						}
					}
					else
					{
						if (var_expansion(line, token, &tok_pos, &i) != 0)
						{
							return(NULL);
						}
						i++;
					}
				}
				break;

			case '\'':
				i++;
				while (line[i] != '\'')
				{
					if (line[i] == '\0')
					{
						fprintf(stderr, "Parser: didn't find closing quote\n");
						return(NULL);
					}
					else
					{
						token[tok_pos] = line[i];
						tok_pos++;
						i++;
					}
				}
				break;

			case '$':
				if (var_expansion(line, token, &tok_pos, &i) != 0)
				{
					return(NULL);
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

					tokens[n] = Malloc(strlen(token) + 1);
					strcpy(tokens[n], token);
					n++;

					if (n >= bufsize)
        			{
            			bufsize *= 2;
            			tokens = Realloc(tokens, bufsize * sizeof(*tokens));
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

		tokens[n] = Malloc(strlen(token) + 1);
		strcpy(tokens[n], token);
		n++;
		if (n >= bufsize)
    	{
    		bufsize *= 2;
        	tokens = Realloc(tokens, bufsize * sizeof(*tokens));
    	}
	}
	tokens[n] = NULL;
	return(tokens);
}


int var_expansion(char *line, char *token, unsigned int *tok_pos, int *i)
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
				fprintf(stderr, "Parser: invalid character in an env var\n");
				return(1);
			}
		}
		(*i)--;
		var_buffer[var_pos] = '\0';
		const char *dollar_var = getenv(var_buffer + 1);
		if (dollar_var != NULL)
		{
			for (int d_pos = 0; dollar_var[d_pos] != '\0'; d_pos++)
			{
				token[*tok_pos] = dollar_var[d_pos];
				(*tok_pos)++;
			}
		}
		else
		{
			fprintf(stderr, "getenv: returned NULL\n");
			return(1);
		}
	}
	return(0);
}
