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
				char var_buffer[256];
				unsigned int var_pos = 0;

				var_buffer[var_pos] = line[i];
				var_pos++;
				i++;

				while (line[i] != '\0' && !isspace((unsigned char)line[i]))
				{
					if (isupper(line[i]) || isdigit(line[i]) || line[i] == '_')
					{
						var_buffer[var_pos] = line[i];
						var_pos++;
						i++;
					}
					else
					{
						fprintf(stderr, "Parser: invalid character in an env var\n");
						read_line();
					}
				}
				i--;
				const char *dollar_var = getenv(var_buffer + 1);
				for (int d_pos = 0; dollar_var[d_pos] != '\0'; d_pos++)
				{
					token[position] = dollar_var[d_pos];
					position++;
				}
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
	return(tokens);
}
