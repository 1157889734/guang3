#include "include.h"
//#define LOG_PATH		"/root/usr_exe/log.ini"

int g_writeRepairedErr[30] = {0};
pthread_mutex_t error_mutex;
//收到数据的内容
extern NVRT_TO_VVS_CONTENT_T g_nvrtToVvsContent;
extern char *error_message_recover[23];

void __ScanfErrorLog(int num,char *result[],t_errorLog *errorLog[]);


int getTime(char *out, int fmt)                 // ???????????
{
    if(out == NULL)
        return -1;
 
    time_t t;
    struct tm *tp;
    t = time(NULL);
 
    tp = localtime(&t);
    if(fmt == 0)
        sprintf(out, "%2.2d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
    else if(fmt == 1)
        sprintf(out, "%2.2d-%2.2d-%2.2d", tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday);
    else if(fmt == 2)
        sprintf(out, "%2.2d:%2.2d:%2.2d", tp->tm_hour, tp->tm_min, tp->tm_sec);
    return 0;
}
 
FILE* openFile(const char *fileName, const char *mode)  // ????????
{
    FILE *fp = fopen(fileName, mode);
    return fp;
}

int closeFile(FILE *fp)
{
    return fclose(fp);
}


int writeFile(FILE *fp, const char *str, int bLog)          // д??????????,bLog??????????????
{
    assert(fp != NULL && str != NULL);
    char curTime[100] = {0};
    int ret = -1;
    if(bLog) // ???????????
    {
        getTime(curTime, 0);
        ret = fprintf(fp, "[%s] %s\n", curTime, str);
    }
    else
        ret = fprintf(fp, "%s\n", str);
 
    if(ret >= 0)
    {
        fflush(fp);
        return 0;               // д??????
    }
    else
        return -1;
}




void ScanfErrorLog(int num,char *result[],t_errorLog *errorLog[])
{
	for(int j=0;j<num;j++)
	{	//dva1
		if(strstr(result[j],"_DVA1"))
		{
			errorLog[0]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[0]->ErrTime);//最后的发生时间
			//sscanf(result[j],"%*[^]]] %s",Dva1Content);//最后的发生内容
		}
		else if(strstr(result[j],"DVA1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[0]->RepTime);//最后的恢复时间
		}	
		//dva2
		else if(strstr(result[j],"_DVA2"))
		{
			errorLog[1]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[1]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"DVA2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[1]->RepTime);//最后的恢复时间
		}	
		//pisc1
		else if(strstr(result[j],"_PISC1"))
		{
			errorLog[2]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[2]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"PISC1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[2]->RepTime);//最后的恢复时间
		}	
		//pisc2
		else if(strstr(result[j],"_PISC2"))
		{
			errorLog[3]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[3]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"PISC2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[3]->RepTime);//最后的恢复时间
		}
		//_LCU6
		else if(strstr(result[j],"_LCU6"))
		{
			errorLog[4]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[4]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU6_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[4]->RepTime);//最后的恢复时间
		}
		//_LCU5
		else if(strstr(result[j],"_LCU5"))
		{
			errorLog[5]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[5]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU5_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[5]->RepTime);//最后的恢复时间
		}
		//_LCU4
		else if(strstr(result[j],"_LCU4"))
		{
			errorLog[6]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[6]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU4_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[6]->RepTime);//最后的恢复时间
		}
		//_LCU3
		else if(strstr(result[j],"_LCU3"))
		{
			errorLog[7]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[7]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU3_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[7]->RepTime);//最后的恢复时间
		}
		//_LCU2
		else if(strstr(result[j],"_LCU2"))
		{
			errorLog[8]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[8]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[8]->RepTime);//最后的恢复时间
		}
		//_LCU1
		else if(strstr(result[j],"_LCU1"))
		{
			errorLog[9]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[9]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[9]->RepTime);//最后的恢复时间
		}
		//DCP2
		else if(strstr(result[j],"_DCP2"))
		{
			errorLog[10]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[10]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"DCP2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[10]->RepTime);//最后的恢复时间
		}
		//DCP1
		else if(strstr(result[j],"_DCP1"))
		{
			errorLog[11]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[11]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"DCP1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[11]->RepTime);//最后的恢复时间
		}
		//APU2
		else if(strstr(result[j],"_APU2"))
		{
			errorLog[12]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[12]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"APU2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[12]->RepTime);//最后的恢复时间
		}
		//APU1
		else if(strstr(result[j],"_APU1"))
		{
			errorLog[13]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[13]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"APU1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[13]->RepTime);//最后的恢复时间
		}
		//IO6
		else if(strstr(result[j],"_IO6"))
		{
			errorLog[14]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[14]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO6_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[14]->RepTime);//最后的恢复时间
		}
		//IO5
		else if(strstr(result[j],"_IO5"))
		{
			errorLog[15]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[15]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO5_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[15]->RepTime);//最后的恢复时间
		}
		//IO4
		else if(strstr(result[j],"_IO4"))
		{
			errorLog[16]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[16]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO4_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[16]->RepTime);//最后的恢复时间
		}
		//IO3
		else if(strstr(result[j],"_IO3"))
		{
			errorLog[17]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[17]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO3_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[17]->RepTime);//最后的恢复时间
		}
		//IO2
		else if(strstr(result[j],"_IO2"))
		{
			errorLog[18]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[18]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[18]->RepTime);//最后的恢复时间
		}
		//IO1
		else if(strstr(result[j],"_IO1"))
		{
			errorLog[19]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[19]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[19]->RepTime);//最后的恢复时间
		}
		//ATC2
		else if(strstr(result[j],"_ATC2"))
		{
			errorLog[20]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[20]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"ATC2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[20]->RepTime);//最后的恢复时间
		}
		//ATC1
		else if(strstr(result[j],"_ATC1"))
		{
			errorLog[21]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[21]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"ATC1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[21]->RepTime);//最后的恢复时间
		}


	}
	//已经修复
		//DVA1
		if(memcmp(errorLog[0]->RepTime,errorLog[0]->ErrTime,sizeof(errorLog[0]->ErrTime))>0)
		{
			sprintf(errorLog[0]->Buffer," %s  %s     %d      DVA1发生故障",errorLog[0]->ErrTime,errorLog[0]->RepTime,errorLog[0]->Times);
		}
		//未修复
		else if(memcmp(errorLog[0]->RepTime,errorLog[0]->ErrTime,sizeof(errorLog[0]->ErrTime))<0)
		{
			sprintf(errorLog[0]->Buffer," %s                          %d      DVA1发生故障",errorLog[0]->ErrTime,errorLog[0]->Times);
		}
		else//刚开始啥都没有
		{
			sprintf(errorLog[0]->Buffer,"                                              %d      DVA1发生故障",errorLog[0]->Times);
		}
		//DVA2
		if(memcmp(errorLog[1]->RepTime,errorLog[1]->ErrTime,sizeof(errorLog[1]->ErrTime))>0)
		{
			sprintf(errorLog[1]->Buffer," %s  %s     %d      DVA2发生故障",errorLog[1]->ErrTime,errorLog[1]->RepTime,errorLog[1]->Times);
		}
		else if(memcmp(errorLog[1]->RepTime,errorLog[1]->ErrTime,sizeof(errorLog[1]->ErrTime))<0)
		{
			sprintf(errorLog[1]->Buffer," %s                          %d      DVA2发生故障",errorLog[1]->ErrTime,errorLog[1]->Times);
		}
		else
		{
			sprintf(errorLog[1]->Buffer,"                                              %d      DVA2发生故障",errorLog[1]->Times);
		}
		//PISC1
		if(memcmp(errorLog[2]->RepTime,errorLog[2]->ErrTime,sizeof(errorLog[2]->ErrTime))>0)
		{
			sprintf(errorLog[2]->Buffer," %s  %s     %d      PISC1发生故障",errorLog[2]->ErrTime,errorLog[2]->RepTime,errorLog[2]->Times);
		}
		else if(memcmp(errorLog[2]->RepTime,errorLog[2]->ErrTime,sizeof(errorLog[2]->ErrTime))<0)
		{
			sprintf(errorLog[2]->Buffer," %s                          %d      PISC1发生故障",errorLog[2]->ErrTime,errorLog[2]->Times);
		}
		else
		{
			sprintf(errorLog[2]->Buffer,"                                              %d      PISC1发生故障",errorLog[2]->Times);
		}
		//PISC2
		if(memcmp(errorLog[3]->RepTime,errorLog[3]->ErrTime,sizeof(errorLog[3]->ErrTime))>0)
		{
			sprintf(errorLog[3]->Buffer," %s  %s     %d      PISC2发生故障",errorLog[3]->ErrTime,errorLog[3]->RepTime,errorLog[3]->Times);
		}
		else if(memcmp(errorLog[3]->RepTime,errorLog[3]->ErrTime,sizeof(errorLog[3]->ErrTime))<0)
		{
			sprintf(errorLog[3]->Buffer," %s                          %d      PISC2发生故障",errorLog[3]->ErrTime,errorLog[3]->Times);
		}
		else
		{
			sprintf(errorLog[3]->Buffer,"                                              %d      PISC2发生故障",errorLog[3]->Times);
		}
		//LCU6
		if(memcmp(errorLog[4]->RepTime,errorLog[4]->ErrTime,sizeof(errorLog[4]->ErrTime))>0)
		{
			sprintf(errorLog[4]->Buffer," %s  %s     %d      LCU6发生故障",errorLog[4]->ErrTime,errorLog[4]->RepTime,errorLog[4]->Times);
		}
		else if(memcmp(errorLog[4]->RepTime,errorLog[4]->ErrTime,sizeof(errorLog[4]->ErrTime))<0)
		{
			sprintf(errorLog[4]->Buffer," %s                          %d      LCU6发生故障",errorLog[4]->ErrTime,errorLog[4]->Times);
		}
		else
		{
			sprintf(errorLog[4]->Buffer,"                                              %d      LCU6发生故障",errorLog[4]->Times);
		}
		//LCU5
		if(memcmp(errorLog[5]->RepTime,errorLog[5]->ErrTime,sizeof(errorLog[5]->ErrTime))>0)
		{
			sprintf(errorLog[5]->Buffer," %s  %s     %d      LCU5发生故障",errorLog[5]->ErrTime,errorLog[5]->RepTime,errorLog[5]->Times);
		}
		else if(memcmp(errorLog[5]->RepTime,errorLog[5]->ErrTime,sizeof(errorLog[5]->ErrTime))<0)
		{
			sprintf(errorLog[5]->Buffer," %s                          %d      LCU5发生故障",errorLog[5]->ErrTime,errorLog[5]->Times);
		}
		else
		{
			sprintf(errorLog[5]->Buffer,"                                              %d      LCU5发生故障",errorLog[5]->Times);
		}
		//LCU4
		if(memcmp(errorLog[6]->RepTime,errorLog[6]->ErrTime,sizeof(errorLog[6]->ErrTime))>0)
		{
			sprintf(errorLog[6]->Buffer," %s  %s     %d      LCU4发生故障",errorLog[6]->ErrTime,errorLog[6]->RepTime,errorLog[6]->Times);
		}
		else if(memcmp(errorLog[6]->RepTime,errorLog[6]->ErrTime,sizeof(errorLog[6]->ErrTime))<0)
		{
			sprintf(errorLog[6]->Buffer," %s                          %d      LCU4发生故障",errorLog[6]->ErrTime,errorLog[6]->Times);
		}
		else
		{
			sprintf(errorLog[6]->Buffer,"                                              %d      LCU4发生故障",errorLog[6]->Times);
		}
		//LCU3
		if(memcmp(errorLog[7]->RepTime,errorLog[7]->ErrTime,sizeof(errorLog[7]->ErrTime))>0)
		{
			sprintf(errorLog[7]->Buffer," %s  %s     %d      LCU3发生故障",errorLog[7]->ErrTime,errorLog[7]->RepTime,errorLog[7]->Times);
		}
		else if(memcmp(errorLog[7]->RepTime,errorLog[7]->ErrTime,sizeof(errorLog[7]->ErrTime))<0)
		{
			sprintf(errorLog[7]->Buffer," %s                          %d      LCU3发生故障",errorLog[7]->ErrTime,errorLog[7]->Times);
		}
		else
		{
			sprintf(errorLog[7]->Buffer,"                                              %d      LCU3发生故障",errorLog[7]->Times);
		}
		//LCU2
		if(memcmp(errorLog[8]->RepTime,errorLog[8]->ErrTime,sizeof(errorLog[8]->ErrTime))>0)
		{
			sprintf(errorLog[8]->Buffer," %s  %s     %d      LCU2发生故障",errorLog[8]->ErrTime,errorLog[8]->RepTime,errorLog[8]->Times);
		}
		else if(memcmp(errorLog[8]->RepTime,errorLog[8]->ErrTime,sizeof(errorLog[8]->ErrTime))<0)
		{
			sprintf(errorLog[8]->Buffer," %s                          %d      LCU2发生故障",errorLog[8]->ErrTime,errorLog[8]->Times);
		}
		else
		{
			sprintf(errorLog[8]->Buffer,"                                              %d      LCU2发生故障",errorLog[8]->Times);
		}
		//LCU1
		if(memcmp(errorLog[9]->RepTime,errorLog[9]->ErrTime,sizeof(errorLog[9]->ErrTime))>0)
		{
			sprintf(errorLog[9]->Buffer," %s  %s     %d      LCU1发生故障",errorLog[9]->ErrTime,errorLog[9]->RepTime,errorLog[9]->Times);
		}
		else if(memcmp(errorLog[9]->RepTime,errorLog[9]->ErrTime,sizeof(errorLog[9]->ErrTime))<0)
		{
			sprintf(errorLog[9]->Buffer," %s                          %d      LCU1发生故障",errorLog[9]->ErrTime,errorLog[9]->Times);
		}
		else
		{
			sprintf(errorLog[9]->Buffer,"                                              %d      LCU1发生故障",errorLog[9]->Times);
		}
		//DCP2
		if(memcmp(errorLog[10]->RepTime,errorLog[10]->ErrTime,sizeof(errorLog[10]->ErrTime))>0)
		{
			sprintf(errorLog[10]->Buffer," %s  %s     %d      DCP2发生故障",errorLog[10]->ErrTime,errorLog[10]->RepTime,errorLog[10]->Times);
		}
		else if(memcmp(errorLog[10]->RepTime,errorLog[10]->ErrTime,sizeof(errorLog[10]->ErrTime))<0)
		{
			sprintf(errorLog[10]->Buffer," %s                          %d      DCP2发生故障",errorLog[10]->ErrTime,errorLog[10]->Times);
		}
		else
		{
			sprintf(errorLog[10]->Buffer,"                                              %d      DCP2发生故障",errorLog[10]->Times);
		}
		//DCP1
		if(memcmp(errorLog[11]->RepTime,errorLog[11]->ErrTime,sizeof(errorLog[11]->ErrTime))>0)
		{
			sprintf(errorLog[11]->Buffer," %s  %s     %d      DCP1发生故障",errorLog[11]->ErrTime,errorLog[11]->RepTime,errorLog[11]->Times);
		}
		else if(memcmp(errorLog[11]->RepTime,errorLog[11]->ErrTime,sizeof(errorLog[11]->ErrTime))<0)
		{
			sprintf(errorLog[11]->Buffer," %s                          %d      DCP1发生故障",errorLog[11]->ErrTime,errorLog[11]->Times);
		}
		else
		{
			sprintf(errorLog[11]->Buffer,"                                              %d      DCP1发生故障",errorLog[11]->Times);
		}
		//APU2
		if(memcmp(errorLog[12]->RepTime,errorLog[12]->ErrTime,sizeof(errorLog[12]->ErrTime))>0)
		{
			sprintf(errorLog[12]->Buffer," %s  %s     %d      APU2发生故障",errorLog[12]->ErrTime,errorLog[12]->RepTime,errorLog[12]->Times);
		}
		else if(memcmp(errorLog[12]->RepTime,errorLog[12]->ErrTime,sizeof(errorLog[12]->ErrTime))<0)
		{
			sprintf(errorLog[12]->Buffer," %s                          %d      APU2发生故障",errorLog[12]->ErrTime,errorLog[12]->Times);
		}
		else
		{
			sprintf(errorLog[12]->Buffer,"                                              %d      APU2发生故障",errorLog[12]->Times);
		}
		//APU1
		if(memcmp(errorLog[13]->RepTime,errorLog[13]->ErrTime,sizeof(errorLog[13]->ErrTime))>0)
		{
			sprintf(errorLog[13]->Buffer," %s  %s     %d      APU1发生故障",errorLog[13]->ErrTime,errorLog[13]->RepTime,errorLog[13]->Times);
		}
		else if(memcmp(errorLog[13]->RepTime,errorLog[13]->ErrTime,sizeof(errorLog[13]->ErrTime))<0)
		{
			sprintf(errorLog[13]->Buffer," %s                          %d      APU1发生故障",errorLog[13]->ErrTime,errorLog[13]->Times);
		}
		else
		{
			sprintf(errorLog[13]->Buffer,"                                              %d      APU1发生故障",errorLog[13]->Times);
		}
		//IO6
		if(memcmp(errorLog[14]->RepTime,errorLog[14]->ErrTime,sizeof(errorLog[14]->ErrTime))>0)
		{
			sprintf(errorLog[14]->Buffer," %s  %s     %d      IO6发生故障",errorLog[14]->ErrTime,errorLog[14]->RepTime,errorLog[14]->Times);
		}
		else if(memcmp(errorLog[14]->RepTime,errorLog[14]->ErrTime,sizeof(errorLog[14]->ErrTime))<0)
		{
			sprintf(errorLog[14]->Buffer," %s                          %d      IO6发生故障",errorLog[14]->ErrTime,errorLog[14]->Times);
		}
		else
		{
			sprintf(errorLog[14]->Buffer,"                                              %d      IO6发生故障",errorLog[14]->Times);
		}
		//IO5
		if(memcmp(errorLog[15]->RepTime,errorLog[15]->ErrTime,sizeof(errorLog[15]->ErrTime))>0)
		{
			sprintf(errorLog[15]->Buffer," %s  %s     %d      IO5发生故障",errorLog[15]->ErrTime,errorLog[15]->RepTime,errorLog[15]->Times);
		}
		else if(memcmp(errorLog[15]->RepTime,errorLog[15]->ErrTime,sizeof(errorLog[15]->ErrTime))<0)
		{
			sprintf(errorLog[15]->Buffer," %s                          %d      IO5发生故障",errorLog[15]->ErrTime,errorLog[15]->Times);
		}
		else
		{
			sprintf(errorLog[15]->Buffer,"                                              %d      IO5发生故障",errorLog[15]->Times);
		}
		//IO4
		if(memcmp(errorLog[16]->RepTime,errorLog[16]->ErrTime,sizeof(errorLog[16]->ErrTime))>0)
		{
			sprintf(errorLog[16]->Buffer," %s  %s     %d      IO4发生故障",errorLog[16]->ErrTime,errorLog[16]->RepTime,errorLog[16]->Times);
		}
		else if(memcmp(errorLog[16]->RepTime,errorLog[16]->ErrTime,sizeof(errorLog[16]->ErrTime))<0)
		{
			sprintf(errorLog[16]->Buffer," %s                          %d      IO4发生故障",errorLog[16]->ErrTime,errorLog[16]->Times);
		}
		else
		{
			sprintf(errorLog[16]->Buffer,"                                              %d      IO4发生故障",errorLog[16]->Times);
		}
		//IO3
		if(memcmp(errorLog[17]->RepTime,errorLog[17]->ErrTime,sizeof(errorLog[17]->ErrTime))>0)
		{
			sprintf(errorLog[17]->Buffer," %s  %s     %d      IO3发生故障",errorLog[17]->ErrTime,errorLog[17]->RepTime,errorLog[17]->Times);
		}
		else if(memcmp(errorLog[17]->RepTime,errorLog[17]->ErrTime,sizeof(errorLog[17]->ErrTime))<0)
		{
			sprintf(errorLog[17]->Buffer," %s                          %d      IO3发生故障",errorLog[17]->ErrTime,errorLog[17]->Times);
		}
		else
		{
			sprintf(errorLog[17]->Buffer,"                                              %d      IO3发生故障",errorLog[17]->Times);
		}
		//IO2
		if(memcmp(errorLog[18]->RepTime,errorLog[18]->ErrTime,sizeof(errorLog[18]->ErrTime))>0)
		{
			sprintf(errorLog[18]->Buffer," %s  %s     %d      IO2发生故障",errorLog[18]->ErrTime,errorLog[18]->RepTime,errorLog[18]->Times);
		}
		else if(memcmp(errorLog[18]->RepTime,errorLog[18]->ErrTime,sizeof(errorLog[18]->ErrTime))<0)
		{
			sprintf(errorLog[18]->Buffer," %s                          %d      IO2发生故障",errorLog[18]->ErrTime,errorLog[18]->Times);
		}
		else
		{
			sprintf(errorLog[18]->Buffer,"                                              %d      IO2发生故障",errorLog[18]->Times);
		}
		//IO1
		if(memcmp(errorLog[19]->RepTime,errorLog[19]->ErrTime,sizeof(errorLog[19]->ErrTime))>0)
		{
			sprintf(errorLog[19]->Buffer," %s  %s     %d      IO1发生故障",errorLog[19]->ErrTime,errorLog[19]->RepTime,errorLog[19]->Times);
		}
		else if(memcmp(errorLog[19]->RepTime,errorLog[19]->ErrTime,sizeof(errorLog[19]->ErrTime))<0)
		{
			sprintf(errorLog[19]->Buffer," %s                          %d      IO1发生故障",errorLog[19]->ErrTime,errorLog[19]->Times);
		}
		else
		{
			sprintf(errorLog[19]->Buffer,"                                              %d      IO1发生故障",errorLog[19]->Times);
		}
		//ATC2
		if(memcmp(errorLog[20]->RepTime,errorLog[20]->ErrTime,sizeof(errorLog[20]->ErrTime))>0)
		{
			sprintf(errorLog[20]->Buffer," %s  %s     %d      ATC2发生故障",errorLog[20]->ErrTime,errorLog[20]->RepTime,errorLog[20]->Times);
		}
		else if(memcmp(errorLog[20]->RepTime,errorLog[20]->ErrTime,sizeof(errorLog[20]->ErrTime))<0)
		{
			sprintf(errorLog[20]->Buffer," %s                          %d      ATC2发生故障",errorLog[20]->ErrTime,errorLog[20]->Times);
		}
		else
		{
			sprintf(errorLog[20]->Buffer,"                                              %d      ATC2发生故障",errorLog[20]->Times);
		}
		//ATC1
		if(memcmp(errorLog[21]->RepTime,errorLog[21]->ErrTime,sizeof(errorLog[21]->ErrTime))>0)
		{
			sprintf(errorLog[21]->Buffer," %s  %s     %d      ATC1发生故障",errorLog[21]->ErrTime,errorLog[21]->RepTime,errorLog[21]->Times);
		}
		else if(memcmp(errorLog[21]->RepTime,errorLog[21]->ErrTime,sizeof(errorLog[21]->ErrTime))<0)
		{
			sprintf(errorLog[21]->Buffer," %s                          %d      ATC1发生故障",errorLog[21]->ErrTime,errorLog[21]->Times);
		}
		else
		{
			sprintf(errorLog[21]->Buffer,"                                              %d      ATC1发生故障",errorLog[21]->Times);
		}

}

int __readFile(FILE *fp, char *buf,char *result[],int *count,t_errorLog *errorLog[])
{
	int i = 0;
	assert(buf!=NULL);
	if(NULL == fp)return -1;
	buf[i] = fgetc(fp);
	while(feof(fp)==0)
	{	
		i++;
		if(i > MAX_FILE_SIZE)
		{
			char ErrorNameToDelete[100]={0};
			combineNameErrorToDelete(ErrorNameToDelete);
			Unlink(ErrorNameToDelete);
			exit(0);
		}
		buf[i] = fgetc(fp);
	}
	buf[i]='\0';

	char *cutFlag = "\n";
	int num = 0;
	result[num] = strtok(buf,cutFlag);	
	while(result[num]!=NULL)
	{
		num++;
		result[num] = strtok(NULL,cutFlag);
	}
	*count = num;
	
	__ScanfErrorLog(num,result,errorLog);
	return 0;
}

int readLogFile(FILE *fp, char *buf,char *result[],int *count)
{
	int i = 0;
	assert(buf!=NULL);
	if(NULL == fp)return -1;
	buf[i] = fgetc(fp);
	while(feof(fp)==0)
	{	
		i++;
		if(i > MAX_FILE_SIZE)
		{
			char ErrorNameToLog[100]={0};
			combineNameLog(ErrorNameToLog);//前一个月的文件
			Unlink(ErrorNameToLog);//删除前一个月的文件
			exit(0);
		}
		buf[i] = fgetc(fp);
	}
	buf[i]='\0';

	char *cutFlag = "\n";
	int num = 0;
	result[num] = strtok(buf,cutFlag);	
	while(result[num]!=NULL)
	{
		num++;
		result[num] = strtok(NULL,cutFlag);
	}
	*count = num;
	//打印出来看看日志有啥
	for(int j=0;j<num;j++)
	{
		printf("restule[%d]=%s\n",j,result[j]);
	}
	return 0;
}

int readFile(FILE *fp, char *buf,char *result[],int *count,t_errorLog *errorLog[])
{
	int i = 0;
	assert(buf!=NULL);
	if(NULL == fp)return -1;
	buf[i] = fgetc(fp);
	while(feof(fp)==0)
	{	
		i++;
		if(i > MAX_FILE_SIZE)
		{
			char ErrorNameToDelete[100]={0};
			combineNameErrorToDelete(ErrorNameToDelete);//前一个月的文件
			Unlink(ErrorNameToDelete);//删除前一个月的文件
			exit(0);
		}
		buf[i] = fgetc(fp);
	}
	buf[i]='\0';

	char *cutFlag = "\n";
	int num = 0;
	result[num] = strtok(buf,cutFlag);	
	while(result[num]!=NULL)
	{
		num++;
		result[num] = strtok(NULL,cutFlag);
	}
	*count = num;
	
	ScanfErrorLog(num,result,errorLog);
	return 0;
}


void combineNameLog(char *combinename)
{
	time_t t;
    struct tm *tp;
    t = time(NULL);
    tp = localtime(&t);

	sprintf(combinename, "/root/usr_exe/Log/%d-%d-%d.ini", tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
}
void combineNameLogToDelete(char *combinename)
{
	time_t t;
    struct tm *tp;
    t = time(NULL);
	int tmpDay;
    tp = localtime(&t);
	if((tp->tm_mday-7)<7) {
		if((tp->tm_mon+2)<1) {
			tmpDay = 31-(7-(tp->tm_mday));
			sprintf(combinename, "/root/usr_exe/Log/%d-%d-%d.ini", tp->tm_year+1900-1,tp->tm_mon+1,tmpDay);
		}else {
			tmpDay = 31-(7-(tp->tm_mday));
			sprintf(combinename, "/root/usr_exe/Log/%d-%d-%d.ini", tp->tm_year+1900,tp->tm_mon,tmpDay);
		}
	} else {
		sprintf(combinename, "/root/usr_exe/Log/%d-%d-%d.ini", tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday-7);
	}
	printf("Delete Log name is %s\n",combinename);
}

/*
void combineName(char *combinename)
{
	time_t t;
    struct tm *tp;
    t = time(NULL);
    tp = localtime(&t);

	sprintf(combinename, "/root/usr_exe/Log/%d-%d-%d.ini", tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
}
*/
void combineNameError(char *combinename)
{
	time_t t;
    struct tm *tp;
    t = time(NULL);
    tp = localtime(&t);
	

	sprintf(combinename, "/root/usr_exe/ERROR/ERROR-%d-%d.ini", tp->tm_year+1900,tp->tm_mon+1);
}

void combineNameErrorToDelete(char *combinename)
{
	time_t t;
    struct tm *tp;
    t = time(NULL);
    tp = localtime(&t);

	sprintf(combinename, "/root/usr_exe/ERROR/ERROR-%d-%d.ini", tp->tm_year+1900,tp->tm_mon+1);
	printf("%s\n",combinename);
}

int readLog(char *buf,char *result[],int *count)
{
	FILE *fp;
	char DateName[100]={0};
	combineNameLog(DateName);
	
	fp = openFile(DateName,"r+");
	if(fp == NULL)//没有文件的时候，先写一个再读
	{
		writeLog("系统开启");
		fp = openFile(DateName,"r+");
		printf("rebuilt Log File\n");
	}

	if(readLogFile(fp,buf,result,count)!=0)//返回读取到的条数
	{
		printf("readFile error");
		return -1;
	}
	closeFile(fp);
	return 0;
}
//为了第一次修复而读
int __readError(char *buf,char *result[],int *count,t_errorLog *errorLog[])
{
	FILE *fpError;
	char ErrorName[100]={0};
	combineNameError(ErrorName);
	fpError = openFile(ErrorName,"r+");
	if(__readFile(fpError,buf,result,count,errorLog)!=0)//返回读取到的条数
	{
		printf("readFile error");
		return -1;
	}	
	closeFile(fpError);
	return 0;
}

int readError(char *buf,char *result[],int *count,t_errorLog *errorLog[])
{
    pthread_mutex_lock(&error_mutex); 
	FILE *fpError;
	char ErrorName[100]={0};
	combineNameError(ErrorName);
	fpError = openFile(ErrorName,"r+");
	if(fpError == NULL)
	{
		writeError("SystemOpen");
		fpError = openFile(ErrorName,"r+");
		printf("rebuilt Error File\n");
		//return -1;
	}
	if(readFile(fpError,buf,result,count,errorLog)!=0)//返回读取到的条数
	{
		printf("readFile error");
		return -1;
	}	
	closeFile(fpError);
	pthread_mutex_unlock(&error_mutex);
	return 0;
}

int Access( char *pathname, int mode )
{
	return access( (const char *)pathname, mode );
}

int Unlink( const char *pathname )
{
	return unlink( pathname );
}


int writeLog(const char *str)
{
    FILE *fp;
	char LogNameToDelete[100]={0};
	char DateNameLog[100]={0};
	combineNameLog(DateNameLog);
	//combineNameLogToDelete(LogNameToDelete);//前7天
	//Unlink(LogNameToDelete);
	int ret = mkdir("/root/usr_exe/Log/",0777);
	if(ret < 0)printf("mkdired\n");	
    fp = openFile(DateNameLog,"a+"); //新建文件
    writeFile(fp,str,1);
    closeFile(fp);
	return 0;
}
int writeError(const char *str)
{
   // pthread_mutex_lock(&error_mutex); 
    FILE *fpError;
	char ErrorName[100]={0};
	char ErrorNameToDelete[100]={0};
	combineNameError(ErrorName);  
	//combineNameErrorToDelete(ErrorNameToDelete);//前一个月的文件
	int ret = mkdir("/root/usr_exe/ERROR/",0777);
	if(ret < 0)printf("mkdired\n");
    fpError = openFile(ErrorName,"a+"); 

    writeFile(fpError,str,1);
    closeFile(fpError);
	//pthread_mutex_unlock(&error_mutex); 
	return 0;
}

void __ScanfErrorLog(int num,char *result[],t_errorLog *errorLog[])
{
	for(int j=0;j<num;j++)
	{	//dva1
		if(strstr(result[j],"_DVA1"))
		{
			errorLog[0]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[0]->ErrTime);//最后的发生时间
			//sscanf(result[j],"%*[^]]] %s",Dva1Content);//最后的发生内容
		}
		else if(strstr(result[j],"DVA1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[0]->RepTime);//最后的恢复时间
		}	
		//dva2
		else if(strstr(result[j],"_DVA2"))
		{
			errorLog[1]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[1]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"DVA2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[1]->RepTime);//最后的恢复时间
		}	
		//pisc1
		else if(strstr(result[j],"_PISC1"))
		{
			errorLog[2]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[2]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"PISC1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[2]->RepTime);//最后的恢复时间
		}	
		//pisc2
		else if(strstr(result[j],"_PISC2"))
		{
			errorLog[3]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[3]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"PISC2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[3]->RepTime);//最后的恢复时间
		}
		//_LCU6
		else if(strstr(result[j],"_LCU6"))
		{
			errorLog[4]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[4]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU6_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[4]->RepTime);//最后的恢复时间
		}
		//_LCU5
		else if(strstr(result[j],"_LCU5"))
		{
			errorLog[5]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[5]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU5_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[5]->RepTime);//最后的恢复时间
		}
		//_LCU4
		else if(strstr(result[j],"_LCU4"))
		{
			errorLog[6]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[6]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU4_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[6]->RepTime);//最后的恢复时间
		}
		//_LCU3
		else if(strstr(result[j],"_LCU3"))
		{
			errorLog[7]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[7]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU3_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[7]->RepTime);//最后的恢复时间
		}
		//_LCU2
		else if(strstr(result[j],"_LCU2"))
		{
			errorLog[8]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[8]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[8]->RepTime);//最后的恢复时间
		}
		//_LCU1
		else if(strstr(result[j],"_LCU1"))
		{
			errorLog[9]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[9]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"LCU1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[9]->RepTime);//最后的恢复时间
		}
		//DCP2
		else if(strstr(result[j],"_DCP2"))
		{
			errorLog[10]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[10]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"DCP2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[10]->RepTime);//最后的恢复时间
		}
		//DCP1
		else if(strstr(result[j],"_DCP1"))
		{
			errorLog[11]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[11]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"DCP1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[11]->RepTime);//最后的恢复时间
		}
		//APU2
		else if(strstr(result[j],"_APU2"))
		{
			errorLog[12]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[12]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"APU2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[12]->RepTime);//最后的恢复时间
		}
		//APU1
		else if(strstr(result[j],"_APU1"))
		{
			errorLog[13]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[13]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"APU1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[13]->RepTime);//最后的恢复时间
		}
		//IO6
		else if(strstr(result[j],"_IO6"))
		{
			errorLog[14]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[14]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO6_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[14]->RepTime);//最后的恢复时间
		}
		//IO5
		else if(strstr(result[j],"_IO5"))
		{
			errorLog[15]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[15]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO5_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[15]->RepTime);//最后的恢复时间
		}
		//IO4
		else if(strstr(result[j],"_IO4"))
		{
			errorLog[16]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[16]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO4_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[16]->RepTime);//最后的恢复时间
		}
		//IO3
		else if(strstr(result[j],"_IO3"))
		{
			errorLog[17]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[17]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO3_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[17]->RepTime);//最后的恢复时间
		}
		//IO2
		else if(strstr(result[j],"_IO2"))
		{
			errorLog[18]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[18]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[18]->RepTime);//最后的恢复时间
		}
		//IO1
		else if(strstr(result[j],"_IO1"))
		{
			errorLog[19]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[19]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"IO1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[19]->RepTime);//最后的恢复时间
		}
		//ATC2
		else if(strstr(result[j],"_ATC2"))
		{
			errorLog[20]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[20]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"ATC2_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[20]->RepTime);//最后的恢复时间
		}
		//ATC1
		else if(strstr(result[j],"_ATC1"))
		{
			errorLog[21]->Times++;
			sscanf(result[j],"[%[^]]",errorLog[21]->ErrTime);//最后的发生时间
		}
		else if(strstr(result[j],"ATC1_"))
		{
			sscanf(result[j],"[%[^]]",errorLog[21]->RepTime);//最后的恢复时间
		}
	}
	//如果还未有修复，查看一下协议是否已修复，如果是，则写进内存
		//DVA1
		if(memcmp(errorLog[0]->RepTime,errorLog[0]->ErrTime,sizeof(errorLog[0]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.dva1 == 0 )
			{
				writeError(error_message_recover[13]);
				
				printf("-------------------dva1----------------------\n");
			}	
		}
		//DVA2
		if(memcmp(errorLog[1]->RepTime,errorLog[1]->ErrTime,sizeof(errorLog[1]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.dva2 == 0 )
			{
				writeError(error_message_recover[12]);
				printf("-------------------dva2----------------------\n");
			}
		}
		//PISC1
		if(memcmp(errorLog[2]->RepTime,errorLog[2]->ErrTime,sizeof(errorLog[2]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.crck1 == 0 )
			{
				writeError(error_message_recover[1]);
				printf("-------------------pisc1----------------------\n");
			}
		}
		//PISC2
		if(memcmp(errorLog[3]->RepTime,errorLog[3]->ErrTime,sizeof(errorLog[3]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.crck2 == 0 )
			{
				writeError(error_message_recover[0]);
				printf("-------------------pisc2----------------------\n");
			}
		}
		//LCU6
		if(memcmp(errorLog[4]->RepTime,errorLog[4]->ErrTime,sizeof(errorLog[4]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.srck6 == 0 )
			{
				writeError(error_message_recover[2]);
				printf("-------------------lcu6----------------------\n");
			}
		}
		//LCU5
		if(memcmp(errorLog[5]->RepTime,errorLog[5]->ErrTime,sizeof(errorLog[5]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.srck5 == 0 )
			{
				writeError(error_message_recover[3]);
				printf("-------------------lcu5---------------------\n");
			}
		}
		//LCU4
		if(memcmp(errorLog[6]->RepTime,errorLog[6]->ErrTime,sizeof(errorLog[6]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.srck4 == 0 )
			{
				writeError(error_message_recover[4]);
				printf("-------------------lcu4----------------------\n");
			}
		}
		//LCU3
		if(memcmp(errorLog[7]->RepTime,errorLog[7]->ErrTime,sizeof(errorLog[7]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.srck3 == 0 )
			{
				writeError(error_message_recover[5]);
				printf("-------------------lcu3----------------------\n");
			}
		}
		//LCU2
		if(memcmp(errorLog[8]->RepTime,errorLog[8]->ErrTime,sizeof(errorLog[8]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.srck2 == 0 )
			{
				writeError(error_message_recover[6]);
				printf("-------------------lcu2----------------------\n");
			}
		}
		//LCU1
		if(memcmp(errorLog[9]->RepTime,errorLog[9]->ErrTime,sizeof(errorLog[9]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.srck1 == 0 )
			{
				writeError(error_message_recover[7]);
				printf("-------------------lcu1----------------------\n");
			}
		}
		//DCP2
		if(memcmp(errorLog[10]->RepTime,errorLog[10]->ErrTime,sizeof(errorLog[10]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.dcp2 == 0 )
			{
				writeError(error_message_recover[8]);
				printf("-------------------dcp2----------------------\n");
			}
		}
		//DCP1
		if(memcmp(errorLog[11]->RepTime,errorLog[11]->ErrTime,sizeof(errorLog[11]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.dcp1 == 0 )
			{
				writeError(error_message_recover[9]);
				printf("-------------------dcp1----------------------\n");
			}
		}
		//APU2
		if(memcmp(errorLog[12]->RepTime,errorLog[12]->ErrTime,sizeof(errorLog[12]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.apu2 == 0 )
			{
				writeError(error_message_recover[10]);
				printf("-------------------apu2----------------------\n");
			}
		}
		//APU1
		if(memcmp(errorLog[13]->RepTime,errorLog[13]->ErrTime,sizeof(errorLog[13]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.apu1 == 0 )
			{
				writeError(error_message_recover[11]);
				printf("-------------------apu1----------------------\n");
			}
		}
		//IO6
		if(memcmp(errorLog[14]->RepTime,errorLog[14]->ErrTime,sizeof(errorLog[14]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.io6 == 0 )
			{
				writeError(error_message_recover[14]);
				printf("-------------------io6----------------------\n");
			}
		}
		//IO5
		if(memcmp(errorLog[15]->RepTime,errorLog[15]->ErrTime,sizeof(errorLog[15]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.io5 == 0 )
			{
				writeError(error_message_recover[15]);
				printf("-------------------io5----------------------\n");
			}
		}
		//IO4
		if(memcmp(errorLog[16]->RepTime,errorLog[16]->ErrTime,sizeof(errorLog[16]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.io4 == 0 )
			{
				writeError(error_message_recover[16]);
				printf("-------------------io4----------------------\n");
			}
		}
		//IO3
		if(memcmp(errorLog[17]->RepTime,errorLog[17]->ErrTime,sizeof(errorLog[17]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.io3 == 0 )
			{
				writeError(error_message_recover[17]);
				printf("-------------------io3----------------------\n");
			}
		}
		//IO2
		if(memcmp(errorLog[18]->RepTime,errorLog[18]->ErrTime,sizeof(errorLog[18]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.io2 == 0 )
			{
				writeError(error_message_recover[18]);
				printf("-------------------io2----------------------\n");
			}
		}
		//IO1
		if(memcmp(errorLog[19]->RepTime,errorLog[19]->ErrTime,sizeof(errorLog[19]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.io1 == 0 )
			{
				writeError(error_message_recover[19]);
				printf("-------------------io1----------------------\n");
			}
		}
		//ATC2
		if(memcmp(errorLog[20]->RepTime,errorLog[20]->ErrTime,sizeof(errorLog[20]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.atc2 == 0 )
			{
				writeError(error_message_recover[20]);
				printf("-------------------atc2----------------------\n");
			}
		}
		//ATC1
		if(memcmp(errorLog[21]->RepTime,errorLog[21]->ErrTime,sizeof(errorLog[21]->ErrTime))<0)
		{
			if( g_nvrtToVvsContent.errorProc.atc1 == 0 )
			{
				writeError(error_message_recover[21]);
				printf("-------------------atc1----------------------\n");
			}
		}

}

