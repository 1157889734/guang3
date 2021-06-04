#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/statfs.h>
#include <semaphore.h>
#include "sys/termios.h"
#include "sys/msg.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/soundcard.h"

#include "bh_malloc.h"


MEMORY_MGR_t *mem_mgr;


void mem_mgr_init(void)
{
	mem_mgr = (MEMORY_MGR_t *)malloc(sizeof(MEMORY_MGR_t));
	memset((char*)mem_mgr, 0, sizeof(MEMORY_MGR_t));
}


void* bh_malloc(int len, char *file_name, int line_num)
{
	MEMORY_MGR_t *mem_mgr_tmp = mem_mgr;
	while (mem_mgr_tmp)
	{
		if (mem_mgr_tmp->p_next==NULL) break;
		mem_mgr_tmp=mem_mgr_tmp->p_next;
	}
	mem_mgr_tmp->p_next = (MEMORY_MGR_t*)malloc(sizeof(MEMORY_MGR_t));
	mem_mgr_tmp = mem_mgr_tmp ->p_next;
	mem_mgr_tmp ->len = len;
	strcpy(mem_mgr_tmp ->file_name, file_name);
	mem_mgr_tmp ->line_num= line_num;
	mem_mgr_tmp->p_next = NULL;
	mem_mgr_tmp ->mem_malloc_p = malloc(len);

	return mem_mgr_tmp ->mem_malloc_p;
}

void bh_free(int* mem_addr)
{
	MEMORY_MGR_t *mem_mgr_tmp = mem_mgr, *mem_mgr_bak, *mem_mgr_next;
	int*mem_addr_tmp=mem_addr;

	free(mem_addr);
	while (mem_mgr_tmp)
	{
		if (mem_mgr_tmp ->mem_malloc_p == mem_addr_tmp)
		{
			mem_mgr_bak = mem_mgr_tmp;
			mem_mgr_next = mem_mgr_tmp ->p_next;
			mem_mgr_tmp= mem_mgr;
			while (mem_mgr_tmp)
			{
				if (mem_mgr_tmp ->p_next == mem_mgr_bak)
				{
					mem_mgr_tmp ->p_next = mem_mgr_next;
					free(mem_mgr_bak);
					return;
				}
				mem_mgr_tmp = mem_mgr_tmp ->p_next;
			}
			return;
		}
		mem_mgr_tmp = mem_mgr_tmp ->p_next;
	}
	return;
}

void *printf_malloc(void* param)
{
	MEMORY_MGR_t *mem_mgr_tmp;
	unsigned int counter = 0;
	while (1)
	{
		mem_mgr_tmp = mem_mgr;
		counter = 0;
		while (mem_mgr_tmp)
		{
			printf("printf_malloc: %s, %d, %d\n",mem_mgr_tmp->file_name, mem_mgr_tmp->line_num,mem_mgr_tmp->len);
			mem_mgr_tmp=mem_mgr_tmp->p_next;
			counter++;
		}
		printf("***********************************************************\n");
		printf("no free mem is %d\n", counter);
		printf("***********************************************************\n\n\n\n\n\n\n\n\n");
		sleep(6);
	}
	return NULL;
}


void bh_malloc_thread_great()
{
#ifdef CHECK_MALLOC
	printf("CHECK_MALLOC  ===============  opened\n");
	mem_mgr_init();
	static pthread_t printf_malloc_id;
	pthread_create(&printf_malloc_id, NULL, printf_malloc, NULL);
#endif
}


