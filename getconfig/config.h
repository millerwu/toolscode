#include <stdio.h>
#include <string.h>

typedef enum {
	VALUE_TYPE_LONG,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_STRING,
	VALUE_TYPE_MAX
} VALUE_TYPE;

typedef enum {
	ERROR_TYPE_NONE = 0,
	ERROR_TYPE_INVALID_PARAM = -1,
	ERROR_TYPE_VALUE_NOT_FIND = -2
} ERRER_TYPE;

int getConfigValue(const char* path, const char* key, void* value, VALUE_TYPE type);

