#include "include.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

static void *HWatchDog_thread(void *args)
{
	int tmp;
	int hWatchDog = open( "/dev/watchdog", O_RDWR );	
	if ( hWatchDog == -1 )
	{
		printf( "Open watch dog fail.\r\n" );
	}
	
	int timeout = 10;
	if ( ioctl( hWatchDog, WDIOC_SETTIMEOUT, &timeout ) == -1 )
	{
		printf( "Open watch dog ioctl error.\r\n" );
		close( hWatchDog );
	}
	
	while(1)
	{
		tmp = ioctl( hWatchDog, WDIOC_KEEPALIVE, (unsigned long)NULL );
		if(tmp != 0)
		{
			printf("failed: watch dog feed ioctl() !\r\n");
		}							    
		//printf("feed\r\n");
	    sleep(2);	
	}
	
	close(hWatchDog);
	return NULL;
}

void Start_HWatchDog_thread()
{
	static pthread_t HWatchDog_thread_id;
	pthread_create(&HWatchDog_thread_id,NULL,HWatchDog_thread,NULL);
}
