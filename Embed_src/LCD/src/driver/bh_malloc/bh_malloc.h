#ifndef __BH_MALLOC_H__
#define __BH_MALLOC_H__

#if 0//开启自定义malloc链表打印功能
#define CHECK_MALLOC 
#else
#define NO_CHECK_MALLOC
#endif


#ifdef CHECK_MALLOC
#define MALLOC(len)	bh_malloc(len, __FILE__, __LINE__)
#define FREE(ptr)		bh_free((int*)ptr);ptr=NULL
#endif

typedef struct Malloc_info_t
{
	void	*mem_malloc_p;
	int	len;
	char file_name[15];
	int line_num;
	struct Malloc_info_t *p_next;
} MEMORY_MGR_t;


#ifdef __cplusplus
extern "C"{
#endif
void* bh_malloc(int len, char *file_name, int line_num);
void bh_free(int* mem_addr);
void bh_malloc_thread_great();
#ifdef __cplusplus
}
#endif

#endif
