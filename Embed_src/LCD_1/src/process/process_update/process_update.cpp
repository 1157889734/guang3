#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <mtd/mtd-user.h>
#include <getopt.h>
#include <errno.h>

#include "tftp_Client.h"
#include <QString>
#include <QProgressBar>
//#include "debug.h"

extern QProgressBar *progressBar_update_program;
extern QProgressBar *progressBar_update_configuration;


/**********************1M(boot),4M(kernel),27M(rootfs)***********************/
static const int SECTOR_SIZE = 128*1024;
#define FLASH_DEVICE_ROOTFS			"/dev/mtd2"	// 文件系统
#define FLASH_OFFSET_ROOTFS			0
#define FLASH_SIZE_ROOTFS				(25*1024*1024)	/* 文件系统 分配了27M*/

unsigned long get_file_size(const char *path)
{
	unsigned long filesize = -1;
	struct stat statbuff;
	if (stat(path, &statbuff) < 0) {
		return filesize;
	} else {
		filesize = statbuff.st_size;
	}
	return filesize;
}
#define STRERROR_ERRNO strerror(errno)
int mtd_erase( const char *mtdname, unsigned int start, unsigned int size )
{
	int fd;
	struct mtd_info_user mtd;
	struct erase_info_user erase;

	fd = open( mtdname, O_RDWR );
	if (fd < 0)
	{
		printf( "Open(%s) error:%s!\n", mtdname, STRERROR_ERRNO );
		return -1;
	}
	if ( ioctl(fd,MEMGETINFO,&mtd) < 0 )
	{
		printf( "ioctl() error:%s!\r\n", STRERROR_ERRNO );
		close( fd );
		return -1;
	}
	if ( (start+size)>mtd.size || 0!=(start%mtd.erasesize) )
	{
		printf( "failed:start+size(%d))>mtd.size(%d) || start(%d)%mtd.erasesize(%d)!=0!\r\n",
		        start+size,mtd.size,start,mtd.erasesize );
		close( fd );
		return -1;
	}
	erase.start 	= start;
	erase.length 	= (size + mtd.erasesize - 1) / mtd.erasesize;
	erase.length 	*= mtd.erasesize;

	if ( ioctl(fd,MEMERASE,&erase) < 0 )
	{
		printf( "ioctl()error:%s!\n", STRERROR_ERRNO );
		close( fd );
		return -1;
	}
	close( fd );
	return 0;
}

int configuration_write( const char *configuration_name,unsigned int size, char *buffer )
{
	int fd;
	int result;

	fd = open( configuration_name, O_RDWR|O_CREAT|O_TRUNC );
	if (fd < 0){
		return -1;
	}
	result = write( fd, buffer, size );
	if ( result<0 || (unsigned int)result != size )
	{
		close( fd );
		return -1;
	}

	close(fd);
	return 0;
}

int file_write( const char *file_name,unsigned int start,unsigned int size, char *buffer )
{
	int fd;
	int result;

	fd = open( file_name, O_RDWR|O_CREAT);
	if (fd < 0){
		printf("open fail\n");
		return -1;
	}
	chmod("/usr/src",S_IXUSR|S_IWUSR|S_IRUSR);
	if ( lseek(fd, start, SEEK_SET) < 0 ){
		close(fd);
		printf("lseek fail\n");
		return -1;
	}
	result = write( fd, buffer, size );
	if ( result<0 ){
		close( fd );
		printf("write fail\n");
		return -1;
	}
	close(fd);
	return 0;
}


int mtd_write( const char *mtdname, unsigned int start, unsigned int size, char *buffer )
{
	int fd;
	int result;
	struct mtd_info_user mtd;

	fd = open( mtdname, O_RDWR );
	if (fd < 0)
	{
		return -1;
	}
	if ( ioctl(fd,MEMGETINFO,&mtd) < 0 )
	{
		close( fd );
		return -1;
	}
	if ((start+size)>mtd.size || 0!=(start%mtd.erasesize))
	{
		close( fd );
		return -1;
	}

	if ( lseek(fd, start, SEEK_SET) < 0 )
	{
		close(fd);
		return -1;
	}
	result = write( fd, buffer, size );
	if ( result<0 || (unsigned int)result != size )
	{
		close( fd );
		return -1;
	}

	close(fd);
	return 0;
}

int Readn( int fd, void *buf, unsigned int size )
{
	unsigned int nleft;
	int nread;
	char *ptr;

	ptr 	= (char *)buf;
	nleft 	= size;
	while ( nleft > 0 )
	{
		nread = read( fd, ptr, nleft );
		if ( nread < 0 )
		{
			if ( errno == EINTR )
			{
				nread = 0; /* and call read() again */
			}
			else
			{
				return -1;
			}
		}
		else if ( 0 == nread )
		{
			break;
		}
		else
		{
			nleft 	-= nread;
			ptr 	+= nread;
			usleep( 10 );
		}
	}

	return( size - nleft );
}

int mtd_read( const char *mtdname, unsigned int start, unsigned int size, char *buffer )
{
	int fd;
	int result;

	fd = open( mtdname, O_RDONLY );
	if ( fd < 0 )
	{
		return -1;
	}
	if (lseek(fd,start,SEEK_SET) < 0)
	{
		close(fd);
		return -1;
	}
	result = Readn( fd, buffer, size );
	if (result<0 || (unsigned int)result != size)
	{
		close( fd );
		return -1;
	}

	close( fd );
	return 0;
}

/*
* fn: 写内容到flash 通用函数
* mtd: mtd 分区, 例如FLASH_DEVICE_CONFIG
* offset: 从该分区的偏移那个地方开始写入
* pData: 指向要写入的数据
* nLen: 要写入多长的数据
* 说明: 写入的时候一次操作一个扇区(这个flash的扇区为SECTOR_SIZE),
		写入前先读出来,如果数据一样,则不操作; 否则，则先擦出,然后写入
* 返回: -1,操作失败; 0,操作成功,但要写入的数据和flash 里面的一样,没有真正写入;
		1,操作成功,且有真正擦写flash的动作
*/
int WriteFlash( char *mtd, int offset, char *pData, int nLen )
{
	int i = 0, j = 0, nRet = -1;
	int start = offset / SECTOR_SIZE * SECTOR_SIZE;

	char *pWriteData = ( char * )malloc( SECTOR_SIZE );
	char *pReadData = ( char * )malloc( SECTOR_SIZE );
	if ( pWriteData == NULL || pReadData == NULL )
	{
		free( pWriteData );
		free( pReadData );
		return nRet;
	}

	if ( offset >= 0 && nLen > 0 && pData )
	{
		nRet = 0;
		offset -= start;
		while ( nLen > 0 )
		{
			int len = nLen;
			if ( (offset+len) > SECTOR_SIZE ) len = SECTOR_SIZE-offset;

			if (mtd_read( mtd, start, SECTOR_SIZE, pWriteData ) < 0)
			{
				free( pWriteData );
				free( pReadData );
				printf( "read flash partion %s offset %08X fail\r\n", mtd, start );
				return -1;
			}
			if ( memcmp(pWriteData+offset, pData, len) )
			{
				nRet = 1;
				memcpy( pWriteData+offset, pData, len );
				for ( j = 0; j < 10; j++ )
				{
					for ( i = 0; i < 10; i++ )
					{
						if ( mtd_erase(mtd, start, SECTOR_SIZE) == 0 ) break;
					}
					if ( i == 10 )
					{
						printf( "Erase flash partion %s offset %08X fail\r\n", mtd, start );
						nRet = -1;
						break;
					}

					if ( mtd_write( mtd, start, SECTOR_SIZE, pWriteData ) == 0 )
					{
						mtd_read( mtd, start, SECTOR_SIZE, pReadData );
						if ( memcmp( pWriteData, pReadData, SECTOR_SIZE ) == 0 ) break;
					}
				}
				if ( nRet == -1 ) break;
				if ( j == 10 )
				{
					printf( "Write flash partion %s offset %08X fail\r\n", mtd, start );
					nRet = -1;
					break;
				}
				usleep( 20*1000 );
			}
			else
			{
				usleep( 20*1000 );
			}
			start += SECTOR_SIZE;
			pData += len;
			nLen -= len;
			offset = 0;
		}
	}

	free( pWriteData );
	free( pReadData );

	return nRet;
}

// jffs2需要全部擦除
int UpgradeRootfs( char *pData, int nLen )
{
	int nRet = -1;
	int offset;
	int align;
	int end;

	if ( 0 < nLen && nLen <= FLASH_SIZE_ROOTFS )
	{
		printf( "Upgrading file system, please wait ...\r\n" );
		nRet = WriteFlash( (char *)FLASH_DEVICE_ROOTFS, FLASH_OFFSET_ROOTFS, pData, nLen );
		if (nRet == -1)
		{
			printf("WriteFlash error\n");
		}
	}

	// 如果写入不满 FLASH_SIZE_ROOTFS, 没有写入的需擦除.
	if (nRet != -1)
	{
		offset = FLASH_OFFSET_ROOTFS + nLen;
		align = offset % SECTOR_SIZE;
		end = FLASH_OFFSET_ROOTFS + FLASH_SIZE_ROOTFS;
		if (align != 0)
		{
			offset = offset + SECTOR_SIZE - align;
		}
		for (; offset < end; offset += SECTOR_SIZE)
		{
			mtd_erase((char *)FLASH_DEVICE_ROOTFS, offset, SECTOR_SIZE);
		}
		printf( "\r\nUpgrading file system %s\r\n", (nRet >= 0) ? "OK" : "FAIL" );
	}

	return nRet;
}

//读取本地文件进行更新
#if 1
int update_from_local(char *file_path)
{
#if 0 //将文件全部读到内存后再更新
	int ret = -1;
	int filesize  = get_file_size(file_path);
	if (filesize > 0)
	{
		char *buffer = (char *)malloc(filesize);
		int read_fd = open(file_path,  O_RDONLY);
		if (read_fd > 0)
		{
			int result = Readn(read_fd, buffer, filesize);
			printf("Readn result = %d\n", result);
			if (result<0 || result != filesize)
			{
				close(read_fd);
				free(buffer);
				return -1;
			}
		}
		close(read_fd);
		if (filesize % 65536 != 0)
		{
			char *real_buffer = (char *)realloc(buffer, filesize + (65536-filesize % 65536));
			memset(&real_buffer[filesize], 0xff, (65536-filesize % 65536));
			printf("real_buffer: %d\n", filesize + (65536-filesize % 65536));
			ret = UpgradeRootfs(real_buffer, filesize + (65536-filesize % 65536));
			free(real_buffer);
			return ret;
		}
		else
		{
			printf("buffer: %d\n", filesize);
			ret = UpgradeRootfs(buffer, filesize);
			free(buffer);
			return ret;
		}
	}
	return -1;
#else //边读边更新，每次读取512bytes
	int filesize  = get_file_size(file_path);
	if(filesize > 0)
	{
		if(filesize > FLASH_SIZE_ROOTFS)
		{
			printf("filesize[%d]  >  FLASH_SIZE_ROOTFS[%d]\n", filesize, FLASH_SIZE_ROOTFS);
			return -1;
		}
		printf("filesize = %d\n", filesize);
	}
	else
	{
		printf("get_file_size error %d\n", filesize);
		return -1;
	}
	int file_read_end = 0;
	int read_fd = open(file_path,  O_RDONLY);
	if (read_fd > 0)
	{
		//文件系统扇区个数
		int count = FLASH_SIZE_ROOTFS / SECTOR_SIZE;

		//progressBar_update_program->setRange(0, count);
               
		//一个扇区一个扇区的檫除
		for (int i=0; i<count; i++)
		{
			//progressBar_update_program->setValue(i);
			if (file_read_end == 0)//文件还没有读完 ==1时就读完文件了
			{
				int pos = 0;
				char *buffer = (char *)malloc(SECTOR_SIZE);
				memset(buffer, 0xff, SECTOR_SIZE);
				for (int j=0; j<256; j++) //每次操作一个扇区128*1024  即512*256 //128k 读文件到一个扇区
				{
					char *read_buf = (char*)malloc(512);
					int result = Readn(read_fd, read_buf, 512);
					if (result < 0)
					{
						close(read_fd);
						free(read_buf);
						free(buffer);
						return -1;
					}
					else if (result == 512)
					{
						memcpy(&buffer[pos], read_buf, 512);
						free(read_buf);
						pos+=512;
					}
					else if (result < 512)
					{
						memcpy(&buffer[pos], read_buf, result);
						free(read_buf);
						file_read_end = 1;
						break;
					}
				}
				for (int try_write= 0; try_write < 10; try_write++ )
				{
					for (int try_erase = 0; try_erase < 10; try_erase++ )
					{
						if ( mtd_erase((char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE) == 0 )
						{
							break;
						}
						else
						{
							if ( try_erase == 9 )
							{
								printf( "Erase flash partion %s offset %08X fail\r\n", (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE );
								close(read_fd);
								free(buffer);
								return -1;
							}
						}
					}
					if ( mtd_write( (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE, buffer ) == 0 )
					{
						char *pReadData = (char*)malloc(SECTOR_SIZE);
						mtd_read(  (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE, pReadData );
						if ( memcmp( buffer, pReadData, SECTOR_SIZE ) == 0 ) 
						{
							free(pReadData);
							break;
						}
						else
						{
							free(pReadData);
							if(try_write == 9)
							{
								printf( "Write flash partion %s offset %08X fail\r\n", (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE );
								close(read_fd);
								free(buffer);
								return -1;
							}
						}
					}
				}
				free(buffer);
				printf("update [erase and write] 0x%08X  size:%d....done\n", i*SECTOR_SIZE, SECTOR_SIZE );
			}
			else //文件读完了
			{
				for (int try_erase = 0; try_erase < 10; try_erase++ )
				{
					if ( mtd_erase((char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE) == 0 )
					{
						break;
					}
					else
					{
						if ( try_erase == 9 )
						{
							printf( "Erase flash partion %s offset %08X fail\r\n", (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE );
							close(read_fd);
							return -1;
						}
					}
				}
				printf("update [erase] 0x%08X size:%d....done\n", i*SECTOR_SIZE, SECTOR_SIZE );
			}
			usleep( 20*1000 );
		}
		//progressBar_update_program->setValue(count);
		close(read_fd);
	}
	else
	{
		printf("open file %s filed\n", file_path);
	}
	return 0;
#endif
}

int updata_from_tftp_file(char *tftp_server_ip, char *file_name,char *download_path)
{
	int filesize = 0;
	int ret = 0,i=0;
	char *file_buf = tftp_receive(tftp_server_ip,file_name,&filesize);
	//printf("filesize = %d\n", filesize);
	int count = filesize/SECTOR_SIZE;// 128k
	if(count<1){
		progressBar_update_program->reset();
		progressBar_update_program->setRange(0, 1);
	}else{
		progressBar_update_program->reset();
		progressBar_update_program->setRange(0, count);
	}
	int file_read_end = 0, read_len = 0;
	if(file_buf!=NULL){
		while(file_read_end == 0)
		{
			char *buffer = (char *)malloc(SECTOR_SIZE);
			memset(buffer, 0xff, SECTOR_SIZE);

			if(read_len + SECTOR_SIZE > filesize){
				file_read_end = 1;
				memcpy(buffer, &file_buf[read_len], filesize - read_len);
				if(file_write(download_path,i*SECTOR_SIZE,filesize - read_len,buffer)){
					printf("download is faile!");
				}else{
					read_len += (filesize - read_len);
					i++;
					progressBar_update_program->setValue(i);
					printf("download is success read_len=%d,filesize=%d\n",read_len,filesize);
				}
			}else{
				memcpy(buffer, &file_buf[read_len], SECTOR_SIZE);
				read_len += SECTOR_SIZE;
				if(file_write(download_path,i*SECTOR_SIZE,SECTOR_SIZE,buffer)){
					printf("download is faile!");
					free(file_buf);
					free(buffer);
					return -1;
				}else{
					i++;
					progressBar_update_program->setValue(i);
					printf("download is success read_len=%d,filesize=%d\n",read_len,filesize);
				}
			}
		}
	}else{
	printf("file_buf is NULL\n");
	free(file_buf);
	return -1;
}
	free(file_buf);
	return 0;
}


//读取tftp 文件进行更新
int update_from_tftp(char *tftp_server_ip, char *file_name)
{
	int filesize = 0;
	//打开tftp客户端一直读取
	char *file_buf = tftp_receive(tftp_server_ip, file_name, &filesize);
	printf("filesize = %d\n", filesize);
	
	int file_read_end = 0, read_len = 0;
	if (file_buf != NULL)
	{
		//文件系统扇区个数
		int count = FLASH_SIZE_ROOTFS / SECTOR_SIZE;

		//progressBar_update_program->setRange(0, count);
               
		printf("count=%d\n",count);
		
#if 1
		for (int i=0; i<count; i++)
		{
			//progressBar_update_program->setValue(i);
			if (file_read_end == 0)//文件还没有读完
			{
				char *buffer = (char *)malloc(SECTOR_SIZE);
				memset(buffer, 0xff, SECTOR_SIZE);

				if(read_len + SECTOR_SIZE > filesize)
				{
					file_read_end = 1;
					memcpy(buffer, &file_buf[read_len], filesize - read_len);
					read_len += filesize - read_len;
					printf("0read_len=%d,filesize=%d\n",read_len,filesize);
				}
				else
				{
					memcpy(buffer, &file_buf[read_len], SECTOR_SIZE);
					read_len += SECTOR_SIZE;
					printf("read_len=%d,filesize=%d\n",read_len,filesize);
				}

				for (int try_write= 0; try_write < 10; try_write++ )
				{
					for (int try_erase = 0; try_erase < 10; try_erase++ )/* 檫除10M */
					{
						if ( mtd_erase((char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE) == 0 )
						{
							break;
						}
						else
						{
							if ( try_erase == 9 )
							{
								printf( "Erase flash partion %s offset %08X fail\r\n", (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE );
								free(file_buf);
								free(buffer);
								return -1;
							}
						}
					}
					if ( mtd_write( (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE, buffer ) == 0 )/* 写1M */
					{
						char *pReadData = (char*)malloc(SECTOR_SIZE);/* 申请1m */
						mtd_read(  (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE, pReadData );/* 读取1m */
						if ( memcmp( buffer, pReadData, SECTOR_SIZE ) == 0 ) 
						{
							free(pReadData);
							break;
						}
						else
						{
							free(pReadData);
							if(try_write == 9)
							{
								printf( "Write flash partion %s offset %08X fail\r\n", (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE );
								free(file_buf);
								free(buffer);
								return -1;
							}
						}
					}
				}
				free(buffer);
				printf("update [erase and write] 0x%08X  size:%d....done\n", i*SECTOR_SIZE, SECTOR_SIZE );
			}
			else //文件读完了
			{
				for (int try_erase = 0; try_erase < 10; try_erase++ )
				{
					if ( mtd_erase((char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE, SECTOR_SIZE) == 0 )
					{
						break;
					}
					else
					{
						if ( try_erase == 9 )
						{
							printf( "Erase flash partion %s offset %08X fail\r\n", (char *)FLASH_DEVICE_ROOTFS, i*SECTOR_SIZE );
							free(file_buf);
							return -1;
						}
					}
				}
				printf("update [erase] 0x%08X size:%d....done\n", i*SECTOR_SIZE, SECTOR_SIZE );
			}
			usleep( 20*1000 );
		}
		//progressBar_update_program->setValue(count);
		free(file_buf);
#endif
	}
	else
	{
		printf("file_buf is NULL\n");
		return -1;
	}
	return 0;
}
#endif
