#include "lib.h"


char **parsing(char *line)
{
	char **tokens;
	size_t bufsize = BUFSIZ;
	unsigned int position = 0;
	unsigned int n = 0;
	char token[256];

	tokens = Malloc(bufsize * sizeof *tokens);

	for (int i = 0; line[i] != '\0'; i++)
	{
		switch (line[i])
		{
			case '$':
				token[position] = line[i];
				position++;
				i++;
				while (line[i] != '\0' && !isspace((unsigned char)line[i]))
				{
					if (isupper(line[i]) || isdigit(line[i]) || line[i] == '_')
					{
						token[position] = line[i];
						position++;
						i++;
					}
					else
					{
						fprintf(stderr, "Parser: invalid character in an env var\n");
						break;
					}
				}
				i--;
				break;

			default:
				if (isspace((unsigned char)line[i]))
				{
					token[position] = '\0';

					tokens[n] = Malloc(strlen(token) + 1);
					strcpy(tokens[n], token);
					n++;

					if (n >= bufsize)
        			{
            			bufsize *= 2;
            			tokens = Realloc(tokens, bufsize * sizeof(*tokens));
        			}
        			memset(token, 0, sizeof(token));
        			position = 0;
        		}
        		else
        		{
        			token[position] = line[i];
        			position++;
        		}
		}
	}
	if (position > 0)
	{
		token[position] = '\0';

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
	var_expand(tokens);
	return(tokens);
}


void var_expand(char **tokens)
{
	for (int i = 1; tokens[i] != NULL; i++)
	{
		const char *dollar_var = strchr(tokens[i], '$');

		if (dollar_var != NULL)
		{
			tokens[i] = getenv(dollar_var + 1);
		}
	}
}
