#include <stdio.h>
#include <string.h>

int main() 
{
	char c1[6];
	char c2[] = "string";
	strcpy(c1, c2);
	printf("c1 = %-5s, c1 size = %d, length = %d\n", c1, sizeof(c1), strlen(c1));
	return 0;
}