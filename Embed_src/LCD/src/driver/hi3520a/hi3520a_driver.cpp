#include "include.h"

static struct fb_bitfield g_r16 = {10, 5, 0};
static struct fb_bitfield g_g16 = {5, 5, 0};
static struct fb_bitfield g_b16 = {0, 5, 0};
static struct fb_bitfield g_a16 = {15, 1, 0};
static HI_S32 s_s32MemDev = 0;

typedef struct hiPTHREAD_HIFB_SAMPLE
{
	int fd;
	int layer;
	int ctrlkey;
}PTHREAD_HIFB_SAMPLE_INFO;

typedef enum
{
	HIFB_LAYER_0 = 0x0,
	HIFB_LAYER_1,
	HIFB_LAYER_2,
	HIFB_LAYER_CURSOR_0,
	HIFB_LAYER_ID_BUTT
} HIFB_LAYER_ID_E;

void SAMPLE_VIO_HandleSig(HI_S32 signo)
{
	if (SIGINT == signo || SIGTSTP == signo)
	{
		SAMPLE_COMM_SYS_Exit();
		printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
	}
	exit(-1);
}

HI_S32 memopen( void )
{
	if (s_s32MemDev <= 0)
	{
		s_s32MemDev = open ("/dev/mem", O_CREAT|O_RDWR|O_SYNC);
		if (s_s32MemDev <= 0)
		{
			return -1;
		}
	}
	return 0;
}

HI_VOID memclose()
{
	close(s_s32MemDev);
}

void * memmap( HI_U32 u32PhyAddr, HI_U32 u32Size )
{
	HI_U32 u32Diff;
	HI_U32 u32PagePhy;
	HI_U32 u32PageSize;
	HI_U8 * pPageAddr;

	u32PagePhy = u32PhyAddr & 0xfffff000;
	u32Diff = u32PhyAddr - u32PagePhy;

	/* size in page_size */
	u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000) + 0x1000;
	pPageAddr = (HI_U8 *)mmap ((void *)0, u32PageSize, PROT_READ|PROT_WRITE,
	                           MAP_SHARED, s_s32MemDev, u32PagePhy);
	if (MAP_FAILED == pPageAddr )
	{
		return NULL;
	}
	return (void *) (pPageAddr + u32Diff);
}

HI_S32 memunmap(HI_VOID* pVirAddr, HI_U32 u32Size )
{
	HI_U32 u32PageAddr;
	HI_U32 u32PageSize;
	HI_U32 u32Diff;

	u32PageAddr = (((HI_U32)pVirAddr) & 0xfffff000);
	/* size in page_size */
	u32Diff     = (HI_U32)pVirAddr - u32PageAddr;
	u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000) + 0x1000;

	return munmap((HI_VOID*)u32PageAddr, u32PageSize);
}

int sample_mpp_init()
{
	VB_CONF_S stVbConf;
	memset(&stVbConf,0,sizeof(VB_CONF_S));

	HI_U32  u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_ENCODING_MODE_PAL,\
	                     PIC_HD720, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	stVbConf.u32MaxPoolCnt = 128;

	stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt = 8;

	int s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return -1;
	}
	return 0;
}

int sample_vpss_start(int VpssGrp, SIZE_S stSize)
{
	int s32Ret;
	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stSize, VPSS_MAX_CHN_NUM,NULL);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start Vpss failed!\n");
		return HI_FAILURE;
	}
	int j;
	for (j=0; j<VPSS_MAX_CHN_NUM; j++)
	{
		VPSS_CHN VpssChn = j;
		VPSS_CHN_MODE_S stVpssMode;

		s32Ret = HI_MPI_VPSS_GetChnMode(VpssGrp,VpssChn,&stVpssMode);
		if (s32Ret != HI_SUCCESS)
		{
			return s32Ret;
		}
		stVpssMode.enChnMode = VPSS_CHN_MODE_AUTO;
		stVpssMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
		stVpssMode.u32Width = stSize.u32Width;
		stVpssMode.u32Height = stSize.u32Height;
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp,VpssChn,&stVpssMode);
		if (s32Ret != HI_SUCCESS)
		{
			return s32Ret;
		}
	}
	return HI_TRUE;
}

int sample_vo_start(int VoDev)
{
	VO_PUB_ATTR_S stVoPubAttr;
	stVoPubAttr.enIntfSync =VO_OUTPUT_1024x768_60;
	stVoPubAttr.enIntfType = VO_INTF_VGA;
	stVoPubAttr.u32BgColor = 0x00000000;
	stVoPubAttr.bDoubleFrame = HI_FALSE;

	int s32Ret = SAMPLE_COMM_VO_StartDevLayer(VoDev, &stVoPubAttr, 25);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start SAMPLE_COMM_VO_StartDevLayer failed!\n");
		return HI_FAILURE;
	}

	return HI_TRUE;
}

void sample_vochn_start(int VoDev ,int voChn, MY_RECT_T vo_rect)
{
	VO_CHN_ATTR_S stChnAttr;
	stChnAttr.stRect.s32X  = vo_rect.x;
	stChnAttr.stRect.s32Y       = vo_rect.y;
	stChnAttr.stRect.u32Width   = vo_rect.width;
	stChnAttr.stRect.u32Height  = vo_rect.height;
	stChnAttr.u32Priority       = 0;
	stChnAttr.bDeflicker        = HI_FALSE;
	int s32Ret = HI_MPI_VO_SetChnAttr(VoDev, voChn, &stChnAttr);
	if (s32Ret != HI_SUCCESS)
	{
		printf("%s(%d):failed with %#x!\n",__FUNCTION__,__LINE__,  s32Ret);
		return ;
	}
	s32Ret = HI_MPI_VO_EnableChn(VoDev, voChn);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return ;
	}
}

void sample_vochn_stop(int VoDev ,int voChn)
{
	HI_MPI_VO_DisableChn(VoDev,voChn);
}

int sample_vo_bind_vpss(int VoDev, int VoChn, int VpssGrp)
{
	VPSS_CHN VpssChn_VoHD0 = VPSS_PRE0_CHN;
	int s32Ret = SAMPLE_COMM_VO_BindVpss(VoDev,VoChn,VpssGrp,VpssChn_VoHD0);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start VO failed!\n");
		return HI_FALSE;
	}

	return HI_TRUE;
}

int sample_vo_unbind_vpss(int VoDev, int VoChn, int VpssGrp)
{
	VPSS_CHN VpssChn_VoHD0 = VPSS_PRE0_CHN;
	int s32Ret = SAMPLE_COMM_VO_UnBindVpss(VoDev,VoChn,VpssGrp,VpssChn_VoHD0);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VO_UnBindVpss failed!\n");
		return HI_FALSE;
	}

	return HI_TRUE;
}

int sample_hifb_start()
{
	PTHREAD_HIFB_SAMPLE_INFO pstInfo;
	pstInfo.layer   =  0;
	pstInfo.fd      = -1;
	pstInfo.ctrlkey =  2;
	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;
	HIFB_ALPHA_S stAlpha;
	HIFB_POINT_S stPoint = {0, 0};
	char file[12] = "/dev/fb0";
	HI_BOOL g_bCompress = HI_FALSE;
	HI_BOOL bShow;
	HIFB_COLORKEY_S stColorKey;
	switch (pstInfo.layer)
	{
	case 0 :
		strcpy(file, "/dev/fb0");
		break;
	case 1 :
		strcpy(file, "/dev/fb1");
		break;
	case 2 :
		strcpy(file, "/dev/fb2");
		break;
	case 3 :
		strcpy(file, "/dev/fb3");
		break;
	default:
		strcpy(file, "/dev/fb0");
		break;
	}

	pstInfo.fd = open(file, O_RDWR, 0);
	if (pstInfo.fd < 0)
	{
		printf("open %s failed!\n",file);
		return HI_FALSE;
	}

	if (pstInfo.layer == HIFB_LAYER_0  )
	{
		if (ioctl(pstInfo.fd, FBIOPUT_COMPRESSION_HIFB, &g_bCompress) < 0)
		{
			printf("Func:%s line:%d FBIOPUT_COMPRESSION_HIFB failed!\n",
			       __FUNCTION__, __LINE__);
			close(pstInfo.fd);
			return HI_FALSE;
		}
	}

	bShow = HI_FALSE;
	if (ioctl(pstInfo.fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
	{
		printf("FBIOPUT_SHOW_HIFB failed!\n");
		return HI_FALSE;
	}
	stPoint.s32XPos = 0;
	stPoint.s32YPos = 0;
	if (ioctl(pstInfo.fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
	{
		printf("set screen original show position failed!\n");
		close(pstInfo.fd);
		return HI_FALSE;
	}
	stAlpha.bAlphaEnable = HI_FALSE;
	stAlpha.bAlphaChannel = HI_FALSE;
	stAlpha.u8Alpha0 = 0x0;
	stAlpha.u8Alpha1 = 0xff;
	stAlpha.u8GlobalAlpha = 0x80;
	if (ioctl(pstInfo.fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
	{
		printf("Set alpha failed!\n");
		close(pstInfo.fd);
		return HI_FALSE;
	}
	stColorKey.bKeyEnable = HI_TRUE;
	stColorKey.u32Key = 0x0;
	if (ioctl(pstInfo.fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
	{
		printf("FBIOPUT_COLORKEY_HIFB!\n");
		close(pstInfo.fd);
		return HI_FALSE;
	}

	if (ioctl(pstInfo.fd, FBIOGET_VSCREENINFO, &var) < 0)
	{
		printf("Get variable screen info failed!\n");
		close(pstInfo.fd);
		return HI_FALSE;
	}
	//usleep(4*1000*1000);
	var.xres_virtual = 1024;
	var.yres_virtual = 768;
	var.xres = 1024;
	var.yres = 768;
	var.transp= g_a16;
	var.red = g_r16;
	var.green = g_g16;
	var.blue = g_b16;
	var.bits_per_pixel = 16;
	var.activate = FB_ACTIVATE_NOW;

	if (ioctl(pstInfo.fd, FBIOPUT_VSCREENINFO, &var) < 0)
	{
		printf("Put variable screen info failed!\n");
		close(pstInfo.fd);
		return HI_FALSE;
	}

	if (ioctl(pstInfo.fd, FBIOGET_FSCREENINFO, &fix) < 0)
	{
		printf("Get fix screen info failed!\n");
		close(pstInfo.fd);
		return HI_FALSE;
	}
	bShow = HI_TRUE;
	if (ioctl(pstInfo.fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
	{
		printf("FBIOPUT_SHOW_HIFB failed!\n");
		return HI_FALSE;
	}
	return HI_TRUE;
}

HI_S32 sample_create_vdec_chn(HI_S32 s32ChnID, SIZE_S *pstSize, PAYLOAD_TYPE_E enType, VIDEO_MODE_E enVdecMode)
{
	VDEC_CHN_ATTR_S stAttr;
	VDEC_PRTCL_PARAM_S stPrtclParam;
	HI_S32 s32Ret;

	memset(&stAttr, 0, sizeof(VDEC_CHN_ATTR_S));

	stAttr.enType = enType;
	stAttr.u32BufSize = pstSize->u32Height * pstSize->u32Width*3/2;//This item should larger than u32Width*u32Height/2
	stAttr.u32Priority = 1;
	stAttr.u32PicWidth = pstSize->u32Width;
	stAttr.u32PicHeight = pstSize->u32Height;

	switch (enType)
	{
	case PT_H264:
		stAttr.stVdecVideoAttr.u32RefFrameNum = 2;
		stAttr.stVdecVideoAttr.enMode = enVdecMode;
		stAttr.stVdecVideoAttr.s32SupportBFrame = 0;
		break;
	case PT_JPEG:
		stAttr.stVdecJpegAttr.enMode = enVdecMode;
		break;
	case PT_MJPEG:
		stAttr.stVdecJpegAttr.enMode = enVdecMode;
		break;
	default:
		SAMPLE_PRT("err type \n");
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VDEC_CreateChn(s32ChnID, &stAttr);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_CreateChn failed errno 0x%x \n", s32Ret);
		return s32Ret;
	}

	s32Ret = HI_MPI_VDEC_GetPrtclParam(s32ChnID, &stPrtclParam);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_GetPrtclParam failed errno 0x%x \n", s32Ret);
		return s32Ret;
	}

	stPrtclParam.s32MaxSpsNum = 21;
	stPrtclParam.s32MaxPpsNum = 22;
	stPrtclParam.s32MaxSliceNum = 100;
	s32Ret = HI_MPI_VDEC_SetPrtclParam(s32ChnID, &stPrtclParam);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_SetPrtclParam failed errno 0x%x \n", s32Ret);
		return s32Ret;
	}

	s32Ret = HI_MPI_VDEC_StartRecvStream(s32ChnID);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_StartRecvStream failed errno 0x%x \n", s32Ret);
		return s32Ret;
	}

	return HI_SUCCESS;
}

void sample_vdec_start(VDEC_CHN VdChn , SIZE_S stSize )
{
	PAYLOAD_TYPE_E enType = PT_H264;
	VIDEO_MODE_E enVdecMode = VIDEO_MODE_STREAM;
	HI_S32 s32Ret;
	s32Ret = sample_create_vdec_chn(VdChn, &stSize, enType, enVdecMode);
	if (HI_SUCCESS !=s32Ret)
	{
		SAMPLE_PRT("create vdec chn failed!\n");
	}
}

void sample_vdec_stop(VDEC_CHN VdChn)
{

	HI_S32 s32Ret;
	s32Ret = HI_MPI_VDEC_StopRecvStream(VdChn);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_StopRecvStream %d failed errno 0x%x \n", VdChn, s32Ret);
	}
	s32Ret = HI_MPI_VDEC_DestroyChn(VdChn);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_DestroyChn %d failed errno 0x%x \n", VdChn, s32Ret);
	}
}

void sample_vdec_reset(VDEC_CHN VdChn)
{
	HI_S32 s32Ret;
	s32Ret = HI_MPI_VDEC_StopRecvStream(VdChn);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_StopRecvStream %d failed errno 0x%x \n", VdChn, s32Ret);
	}
	HI_MPI_VDEC_ResetChn(VdChn);
	s32Ret = HI_MPI_VDEC_StartRecvStream(VdChn);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VDEC_StartRecvStream failed errno 0x%x \n", s32Ret);
	}
}

void video_frame_change_to_black(VIDEO_FRAME_INFO_S *my_stFrame_out)
{
	char *pVBufVirt_Y_out;
	char *pVBufVirt_UV_out;
	HI_U32 Y_size_out, UV_size_out;
	Y_size_out = my_stFrame_out->stVFrame.u32Stride[0]*my_stFrame_out->stVFrame.u32Height;
	UV_size_out = my_stFrame_out->stVFrame.u32Stride[1]*my_stFrame_out->stVFrame.u32Height/2;
	pVBufVirt_Y_out =  (char *)HI_MPI_SYS_Mmap(my_stFrame_out->stVFrame.u32PhyAddr[0], Y_size_out);
	pVBufVirt_UV_out =  (char *)HI_MPI_SYS_Mmap(my_stFrame_out->stVFrame.u32PhyAddr[1], UV_size_out);
	memset(pVBufVirt_Y_out, 0x00, Y_size_out);
	memset(pVBufVirt_UV_out, 0x80, UV_size_out);
	HI_MPI_SYS_Munmap((char*)pVBufVirt_Y_out, Y_size_out);
	HI_MPI_SYS_Munmap((char*)pVBufVirt_UV_out, UV_size_out);
}

void creat_video_frame(VIDEO_FRAME_INFO_S * stFrame_out, int width, int height)
{
	HI_U32             u32LStride;
	HI_U32             u32CStride;
	HI_U32             u32LumaSize;
	HI_U32             u32ChrmSize;
	HI_U32             u32Size;
	VB_BLK VbBlk;
	HI_U32 u32PhyAddr;
	HI_U8 *pVirAddr;

	int u32Stride = width;
	int u32Height = height;
	int u32Width = width;

	u32LStride  = u32Stride;
	u32CStride  = u32Stride;

	u32LumaSize = (u32LStride * u32Height);
	u32ChrmSize = (u32CStride * u32Height) >> 2;
	u32Size = u32LumaSize + (u32ChrmSize << 1);
	VbBlk = HI_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
	if (VB_INVALID_HANDLE == VbBlk)
	{
		SAMPLE_PRT("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);
		return;
	}
	u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
	if (0 == u32PhyAddr)
	{
		return;
	}

	pVirAddr = (HI_U8 *) HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
	if (NULL == pVirAddr)
	{
		return;
	}
	stFrame_out->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
	if (VB_INVALID_POOLID == stFrame_out->u32PoolId)
	{
		return;
	}
	SAMPLE_PRT("pool id :%d, phyAddr:%x,virAddr:%x\n" ,stFrame_out->u32PoolId,u32PhyAddr,(int)pVirAddr);

	stFrame_out->stVFrame.u32PhyAddr[0] = u32PhyAddr;
	stFrame_out->stVFrame.u32PhyAddr[1] = stFrame_out->stVFrame.u32PhyAddr[0] + u32LumaSize;
	stFrame_out->stVFrame.u32PhyAddr[2] = stFrame_out->stVFrame.u32PhyAddr[1] + u32ChrmSize;

	stFrame_out->stVFrame.pVirAddr[0] = pVirAddr;
	stFrame_out->stVFrame.pVirAddr[1] = stFrame_out->stVFrame.pVirAddr[0] + u32LumaSize;
	stFrame_out->stVFrame.pVirAddr[2] = stFrame_out->stVFrame.pVirAddr[1] + u32ChrmSize;

	stFrame_out->stVFrame.u32Width  = u32Width;
	stFrame_out->stVFrame.u32Height = u32Height;
	stFrame_out->stVFrame.u32Stride[0] = u32LStride;
	stFrame_out->stVFrame.u32Stride[1] = u32CStride;
	stFrame_out->stVFrame.u32Stride[2] = u32CStride;
	stFrame_out->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	stFrame_out->stVFrame.u32Field = VIDEO_FIELD_FRAME;

	HI_MPI_SYS_Munmap(pVirAddr, u32Size);

	video_frame_change_to_black(stFrame_out);
}
