#ifndef __HI3520A_DRIVER_H__
#define __HI3520A_DRIVER_H__

typedef struct myRect
{
	int x;
	int y;
	int width;
	int height;
}MY_RECT_T;

void SAMPLE_VIO_HandleSig(HI_S32 signo);
HI_S32 memopen( void );
HI_VOID memclose();
void * memmap( HI_U32 u32PhyAddr, HI_U32 u32Size );
HI_S32 memunmap(HI_VOID* pVirAddr, HI_U32 u32Size );
int sample_mpp_init();
int sample_vi_bind_vpss(int ViChn, int VpssGrp);
int sample_vo_start(int VoDev);
void sample_vochn_start(int VoDev ,int voChn, MY_RECT_T vo_rect);
void sample_vochn_stop(int VoDev ,int voChn);
int sample_vo_bind_vpss(int VoDev, int VoChn, int VpssGrp);
int sample_vo_unbind_vpss(int VoDev, int VoChn, int VpssGrp);
int sample_hifb_start();
int sample_vpss_start(int VpssGrp, SIZE_S stSize);
void sample_vdec_start(VDEC_CHN VdChn , SIZE_S stSize );
void sample_vdec_stop(VDEC_CHN VdChn);
void sample_vdec_reset(VDEC_CHN VdChn);
void video_frame_change_to_black(VIDEO_FRAME_INFO_S *my_stFrame_out);
void creat_video_frame(VIDEO_FRAME_INFO_S * stFrame_out, int width, int height);

#endif
