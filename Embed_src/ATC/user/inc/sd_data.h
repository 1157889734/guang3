
#ifndef __SD_DATA_H
#define __SD_DATA_H

#include"config.h"
#include"rtc_1337.h"
#include"TCMS.h"

#define FILEBUFLEN     512           //? +10ļ???????С
#define CHARFILEBUFLEN FILEBUFLEN     //?ļ??ַ???????С

#define FREAMLEN   512                //֡???󳤶?

#define TMS        0
#define PIS        1
#define TBA        2

#define FRAMEOK    0
#define FRAMEERR   1


#define FREESPACE     1024*100  //ʣ???ռ???Сֵ1024kb*50 =50M
#define FILEMAXSIZE   1024*50 //?ļ??????ֽ???

typedef struct SD_TIME
{
    u8 sec;
    u8 min;
    u8 hour;
} _tsd_time;

typedef struct SD_MODE
{
    uint8 head;            //֡ͷ
    _tsd_time star_time;   //??ʼʱ??
    _tsd_time stop_time;   //????ʱ??
    uint8 num;             //????
    uint8 rhead;           //?ظ?֡ͷ
    uint8 len;             //???ݳ???
    uint8 buf[FREAMLEN];   //????
} _tsd_mode;
typedef struct SD_MODE_PIS
{
    uint8 head;            //֡ͷ
    _tsd_time star_time;   //??ʼʱ??
    uint8 rhead;           //?ظ?֡ͷ
    uint8 len;             //???ݳ???
    uint8 buf[FREAMLEN];   //????
} _tsd_mode_pis;

typedef struct SD_BUF
{
    uint16 len;
    uint8 buf[FILEBUFLEN];
} _tsd_file;

void myatoi(char *src, uint8 *dest, uint16 len);
void myitoa(uint8 *src, char *dest, uint16 len);

/***********************************************************************************************************************
*Function Nane    : get_use_file
*Description      : ?????ļ??Ƿ???ʹ????
*Input            :  path ?ļ?·??
*Output           :  ??
*Return           :  1 ʹ???? 0  ??????
*Others           :  ??
***********************************************************************************************************************/
uint8  check_use_file(char *path);
void form_normal_frame_pis(uint8 type,  tRtcTimer time, uint8 *buf, uint16 len);
/***********************************************************************************************************************
*Function Nane    :  get_sd_wp_state
*Description      :  ??ȡsd??д????״̬
*Input            : ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
uint8 get_sd_wp_state(void);

/***********************************************************************************************************************
*Function Nane   :  get_sd_state
*Description      :  ??ȡsd???Ƿ?????
*Input              : ??
*Output            :  ??
*Return            : 0  ???? 1 û?в???
*Others            :  ??
***********************************************************************************************************************/
uint8 get_sd_state(void);
/***********************************************************************************************************************
*Function Nane   :  get_sd_ready
*Description      : ??ȡsd?Ƿ?׼??????
*Input              : ??
*Output            :  ??
*Return            : 0  û??׼?????? 1 ׼??????
*Others            :  ??
***********************************************************************************************************************/
uint8 get_sd_ready(void);

/***************************************************************************************
*Function Nane  : check_sd_state
*Description      : ????sd ??״̬
*Input              :  ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
*****************************************************************************************/
void check_sd_state(void);

/***********************************************************************************************************************
*Function Nane    :  sd_init
*Description      :  sd????ʼ??
*Input            : ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void sd_init(void);

/***********************************************************************************************************************
*Function Nane  :  update_dir
*Description      :  Ŀ¼
*Input              :  ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void update_dir(void);

/***************************************************************************************
*Function Nane  :  get_sd_space
*Description      :  ??ȡSD???ռ???Ϣ
*Input              :  ??
*Output            :  total ?ܿռ?         ??λ(KB)
                          free   ʣ???ռ?   ??λ(KB)
*Return            :  1 ?ɹ? 0  ʧ??
*Others            :  ??
*****************************************************************************************/
uint8 get_sd_space(uint32 *total, uint32 *free);

/***************************************************************************************
*Function Nane  : get_dir_size
*Description      :  ??ȡĿ¼?ļ??Ĵ?С
*Input              :  ??
*Output            :  path ·??
*Return            :  ?ļ???С
*Others            :  ??
*****************************************************************************************/
uint32 get_dir_size(char *path);

/***************************************************************************************
*Function Nane  : sd_remove_file
*Description      : ɾ???ļ?
*Input              :  path ?ļ?·??
*Output            :  ??
*Return            :  1?ɹ? 0 ʧ??
*Others            :  ??
*****************************************************************************************/
uint8 sd_remove_file(char *path);

/***********************************************************************************************************************
*Function Nane  :  save_start_time
*Description      :  ??¼ϵͳ????ʱ??
*Input              :  ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void save_start_time(void);

/***********************************************************************************************************************
*Function Nane  : save_change_time
*Description      :  ??¼ϵͳ?޸?ʱ??
*Input              :   oldtimer  ??ʱ??
                           newtimer ??ʱ??
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void save_change_time(tRtcTimer oldtimer, tRtcTimer newtimer);

/***********************************************************************************************************************
*Function Nane  :  form_sd_frame
*Description      :  ??֯д??SD????????
*Input              :   type ????         0 : tms       1: pis      2:train485
                           mode ʱ????  0 : ??ʼ 1 ?м?  2:????
                           buf  ????
                           len  ???ݳ???
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void form_normal_frame(uint8 type,  uint8 mode, tRtcTimer time, uint8 *buf, uint16 len);

/***********************************************************************************************************************
*Function Nane  :  form_error_frame
*Description      :  ??֯д??SD????????
*Input              :   type ????         0 : tms       1: pis      2:train485
                           mode ʱ????  0 : ??ʼ 1 ?м?  2:????
                           buf  ????
                           len  ???ݳ???
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void form_error_frame(uint8 type,  uint8 mode, tRtcTimer time, uint8 *buf, uint16 len);

/***********************************************************************************************************************
*Function Nane  :  sd_fflush
*Description      :  ǿ?ư??ļ?????????????д???ļ?
*Input              :   type  ?ļ?????  0 : tms       1: pis      2:tba
                           mode ???????? FRAMEOK : ???? FRAMEERR ????
*Output            :  ??
*Return            :  ??
*Others            :  ??
***********************************************************************************************************************/
void sd_fflush(uint8 type, uint8 mode);

void check_sd_space(void);

void update_rec_data_dir(void);


void SD_LED(void);


#define MVB_REC 0
#define MVB_SEND 1
#define PIS_REC 2
#define PIS_SEND 3
#define OP_LOG 4

void save_normal_data_pis(uint8 type, uint8 *buf, uint16 len);
#endif
