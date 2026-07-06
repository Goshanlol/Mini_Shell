#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *s = "HOME";
	printf(getenv(s));
}
