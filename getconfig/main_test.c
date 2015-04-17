#include <stdio.h>
#include "config.h"

#define TEST_PATH "./test.cfg"


int main()
{
	char value[64] = {0};
	long value_long = 0;
	float value_float = 0.0;

	int res = getConfigValue(TEST_PATH, "string.key", (void*)value, VALUE_TYPE_STRING);
	res = getConfigValue(TEST_PATH, "long.key", (void*)&value_long, VALUE_TYPE_LONG);
	res = getConfigValue(TEST_PATH, "float.key", (void*)&value_float, VALUE_TYPE_FLOAT);

	printf("getConfigValue, string = %s, long = %d, float = %f\n", value, value_long, value_float);
	
	return res;
}
