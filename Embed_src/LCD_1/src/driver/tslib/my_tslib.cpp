#include "include.h"

void start_tslib()
{
	FILE *fd=NULL;
	fd = fopen("/etc/pointercal","r");
	if(fd == NULL)
	{
		system("/root/tslib/bin/ts_calibrate");
	}
	else
	{
		fclose(fd);
		fd=NULL;
	}
}
