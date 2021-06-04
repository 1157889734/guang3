#include "include.h"

#define DDR2_REG_BASE   (0x20000000)
#define MAP_SIZE        4096UL
#define MAP_MASK        (MAP_SIZE - 1)

static unsigned char *pTestRegBase = NULL;
static int dev_fd;

#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)
typedef unsigned long U32;
typedef unsigned char UCHAR;
typedef char CHAR;

int atoul(CHAR *str,U32 * pulValue);
int  atoulx(CHAR *str,U32 * pulValue);

int  atoulx(CHAR *str,U32 * pulValue)
{
    U32   ulResult=0;
    UCHAR ch;

    while (*str)
    {
        ch=toupper(*str);
        if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
        {
            if (ulResult < 0x10000000)
            {
                ulResult = (ulResult << 4) + ((ch<='9')?(ASC2NUM(ch)):(HEXASC2NUM(ch)));
            }
            else
            {
                *pulValue=ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    
    *pulValue=ulResult;
    return HI_SUCCESS;
}

int StrToNumber(CHAR *str , U32 * pulValue)
{
    /*判断是否16进制的字符串*/
    if ( *str == '0' && (*(str+1) == 'x' || *(str+1) == 'X') )
    {
        if (*(str+2) == '\0')
        {
            return HI_FAILURE;
        }
        else
        {
            return atoulx(str+2,pulValue);
        }
    }
    else
    {
        return atoul(str,pulValue);
    }
}

int atoul(CHAR *str,U32 * pulValue)
{
    U32 ulResult=0;

    while (*str)
    {
        if (isdigit((int)*str))
        {
            /*最大支持到0xFFFFFFFF(4294967295), 
               X * 10 + (*str)-48 <= 4294967295
               所以， X = 429496729 */
            if ((ulResult<429496729) || ((ulResult==429496729) && (*str<'6')))
            {
                ulResult = ulResult*10 + (*str)-48;
            }
            else
            {
                *pulValue = ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    *pulValue=ulResult;
    return HI_SUCCESS;
}

//int main(int argc, char *argv[])
int memr_get_value(unsigned long ulAddr)
{
unsigned int reg_value;
    dev_fd = open("/dev/mem", O_RDWR | O_NDELAY);       
    if (dev_fd < 0)  {
        perror("open(/dev/mem) failed.");     
        return -1;
    } 
    
 //   printf("====dump memory %#lX====\n", ulAddr);

	pTestRegBase = (unsigned char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, ulAddr & ~MAP_MASK);

    if (MAP_FAILED == pTestRegBase) {
        printf("mmap failed. fd(%d), addr(0x%x), size(%ld)\n", dev_fd, ulAddr, MAP_SIZE); 
    } else  {
        reg_value = *((volatile unsigned int *)(pTestRegBase + (ulAddr & MAP_MASK)));
       // printf("reg_value = 0x%08x\n", reg_value);
        munmap((void*)pTestRegBase, MAP_SIZE);
    }

    pTestRegBase = NULL;

	
    if(dev_fd)
        close(dev_fd);
    
    return reg_value;
}

//int main(int argc, char *argv[])
int memr_set_value(unsigned long ulAddr,unsigned long ulNew)
{
    unsigned int ulOld;
    unsigned char *pMem =NULL;

    dev_fd = open("/dev/mem", O_RDWR | O_NDELAY);       
    if (dev_fd < 0)  {
        perror("open(/dev/mem) failed.");     
        return -1;
    }
	//printf("map before\r\n");
    //pTestRegBase = (void *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, ulAddr & ~MAP_MASK);
    //?????????????????????????& ffff f000
    pTestRegBase = (unsigned char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, ulAddr & ~MAP_MASK);
	//printf("map ok\r\n");
    if (MAP_FAILED == pTestRegBase) {
        printf("mmap failed. fd(%d), addr(0x%x), size(%d)\n", dev_fd, ulAddr, MAP_SIZE); 
    }else{
    
	//printf("mmap ok. fd(%d), addr(0x%x), size(%d)\n", dev_fd, ulAddr, MAP_SIZE); 
        pMem = pTestRegBase + (ulAddr & MAP_MASK);//&??0000 0fff????????????
        ulOld = *((volatile unsigned int *)pMem);
        /*hi_hexdump(STDOUT, pMem, DEFAULT_MD_LEN, 16);*/
      //  printf("setulAddr %x: 0x%08x --> 0x%08x \n", ulAddr, ulOld, ulNew);
        *(volatile U32*)pMem = ulNew;
        munmap((void*)pTestRegBase, MAP_SIZE);
        pTestRegBase = NULL;
    }

    if(dev_fd >= 0)
    {
    	close(dev_fd);
		printf("\n");
		dev_fd = -1;
    }	
	
    
    return 0;
}