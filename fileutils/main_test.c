#include "fileutils.h"

#define TEST_PATH  "/mnt/hello/h/h2/he22"

int main()
{
	int res = makePath(TEST_PATH, 0777);
	return res;	
}
