#ifndef __LOG_H__
#define __LOG_H__

#include <time.h> 
#include <assert.h>
/*typedef struct _ErrorPro_
{
	int Times;
	char ErrTime[20];
	char RepTime[20];
	char Content[50];
	char Buffer[100];
}ERROR_T;
*/
typedef struct _ERROR_LOG_
{
	int Times;
	char ErrTime[20];
	char RepTime[20];
	char Content[50];
	char Buffer[100];
}t_errorLog;

int writeLog(const char *str);
int readLog(char *buf,char *result[],int *count);

int writeError(const char *str);
int readError(char *buf,char *result[],int *count,t_errorLog *errorLog[]);
int __readError(char *buf,char *result[],int *count,t_errorLog *errorLog[]);
void combineNameLogToDelete(char *combinename);
void combineNameErrorToDelete(char *combinename);
void combineNameLog(char *combinename);
int Unlink( const char *pathname );

#endif
