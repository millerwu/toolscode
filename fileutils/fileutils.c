#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>

#include "fileutils.h"

#define LOG printf

static int do_mkdir(const char* path, int mode)
{
	struct stat st;
	
	if  ( 0 != stat(path, &st) )
	{
		if  ( 0 != mkdir(path, mode) && EEXIST != errno )
		{
		    LOG("fail to mkdir [%s]: %d[%s] \n", path, errno, strerror(errno));
		    return  -1;
		}
	}
	else if ( ! S_ISDIR(st.st_mode) )
	{
		LOG("!S_ISDIR");
		return  -1;
	}
	
	return  0;
}


int makePath(const char* path, int mode)
{
	int res = 0;
	if (path==NULL) {
		LOG("path is null, error\n");
		return -1;
	}
	LOG("path lenght = %d path = %s\n", strlen(path), path);
	char* copypath = (char*)malloc(strlen(path));
	strcpy(copypath, path);
	char* pp = copypath;
	char* sp;
	while ((0 != (sp = strchr(pp, '/')))) {
		LOG("sp = %s \n", sp);
		if (sp!=pp){
			*sp = '\0';
			res = do_mkdir(copypath, mode);
			*sp = '/';
		}
		pp = sp+1;
	}
	if (res>=0) {
		res = do_mkdir(path, mode);
	}
	free(copypath);
	return 0;
}
