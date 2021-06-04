/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_istr.c
* Author             : MCD Application Team
* Version            : V3.0.0
* Date               : 04/06/2009
* Description        : ISTR events interrupt service routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "usb_istr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t wIstr;  /* ISTR register last read value */
__IO uint8_t bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */
static uint8_t is_sending=0;
static uint8_t usb_recv_fifo[USB_FIFO_SIZE];
static uint16_t usb_recv_fifo_in=0;
static uint16_t usb_recv_fifo_out=0;
static uint8_t usb_send_fifo[USB_FIFO_SIZE];
static uint16_t usb_send_fifo_in=0;
static uint16_t usb_send_fifo_out=0;
uint16_t send_cout = 0;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* function pointers to non-control endpoints service routines */
void (*pEpInt_IN[7])(void) =
  {
    EP1_IN_Callback,
    EP2_IN_Callback,
    EP3_IN_Callback,
    EP4_IN_Callback,
    EP5_IN_Callback,
    EP6_IN_Callback,
    EP7_IN_Callback,
  };

void (*pEpInt_OUT[7])(void) =
  {
    EP1_OUT_Callback,
    EP2_OUT_Callback,
    EP3_OUT_Callback,
    EP4_OUT_Callback,
    EP5_OUT_Callback,
    EP6_OUT_Callback,
    EP7_OUT_Callback,
  };


/*******************************************************************************
* Function Name  : USB_Istr
* Description    : STR events interrupt service routine
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void USB_Istr(void)
{

  wIstr = _GetISTR();

#if (IMR_MSK & ISTR_RESET)
  if (wIstr & ISTR_RESET & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_RESET);
    Device_Property.Reset();
#ifdef RESET_CALLBACK
    RESET_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_DOVR)
  if (wIstr & ISTR_DOVR & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_DOVR);
#ifdef DOVR_CALLBACK
    DOVR_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_ERR)
  if (wIstr & ISTR_ERR & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_ERR);
#ifdef ERR_CALLBACK
    ERR_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_WKUP)
  if (wIstr & ISTR_WKUP & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_WKUP);
    Resume(RESUME_EXTERNAL);
#ifdef WKUP_CALLBACK
    WKUP_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_SUSP)
  if (wIstr & ISTR_SUSP & wInterrupt_Mask)
  {

    /* check if SUSPEND is possible */
    if (fSuspendEnabled)
    {
      Suspend();
    }
    else
    {
      /* if not possible then resume after xx ms */
      Resume(RESUME_LATER);
    }
    /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
    _SetISTR((uint16_t)CLR_SUSP);
#ifdef SUSP_CALLBACK
    SUSP_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_SOF)
  if (wIstr & ISTR_SOF & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_SOF);
    bIntPackSOF++;

#ifdef SOF_CALLBACK
    SOF_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_ESOF)
  if (wIstr & ISTR_ESOF & wInterrupt_Mask)
  {
    _SetISTR((uint16_t)CLR_ESOF);
    /* resume handling timing is made with ESOFs */
    Resume(RESUME_ESOF); /* request without change of the machine state */

#ifdef ESOF_CALLBACK
    ESOF_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_CTR)
  if (wIstr & ISTR_CTR & wInterrupt_Mask)
  {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    CTR_LP();
#ifdef CTR_CALLBACK
    CTR_Callback();
#endif
  }
#endif
} /* USB_Istr */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP3_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#if 0
void UserToPMABufferCopy(uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
  uint32_t n = (wNBytes + 1) >> 1;   /* n = (wNBytes + 1) / 2 */
  uint32_t i, temp1, temp2;
  uint16_t *pdwVal;
  pdwVal = (uint16_t *)(wPMABufAddr * 2 + PMAAddr);
  for (i = n; i != 0; i--)
  {
    temp1 = (uint16_t) * pbUsrBuf;
    pbUsrBuf++;
    temp2 = temp1 | (uint16_t) * pbUsrBuf << 8;
    *pdwVal++ = temp2;
    pdwVal++;
    pbUsrBuf++;
  }
}
#endif
/*******************************************************************************
* Function Name  : PMAToUserBufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf    = pointer to user memory area.
*                  - wPMABufAddr = address into PMA.
*                  - wNBytes     = no. of bytes to be copied.
* Output         : None.
* Return         : None.
*******************************************************************************/
#if 0
void PMAToUserBufferCopy(uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
  uint32_t n = (wNBytes + 1) >> 1;/* /2*/
  uint32_t i;
  uint32_t *pdwVal;
  pdwVal = (uint32_t *)(wPMABufAddr * 2 + PMAAddr);
  for (i = n; i != 0; i--)
  {
    *(uint16_t*)pbUsrBuf++ = *pdwVal++;
    pbUsrBuf++;
  }
}
#endif
void EP1_OUT_Callback(void)
{
  //count_out = GetEPRxCount(ENDP3);
  //PMAToUserBufferCopy(buffer_out, ENDP1_RXADDR, count_out);
  //SetEPRxValid(ENDP3);
  uint8_t packet_length;//�����������ݵĳ���
  uint8_t *src;
  src=(uint8_t *)(ENDP1_RXADDR * 2 + PMAAddr);
  packet_length=*((uint16_t *)src);//*((uint16 *)src);//USB_COUNT_RX(EPindex)&0x03FF;//��ȡ�������ݵĳ���
 // if(packet_length>EP1_MAXPACKETSIZE-2)packet_length=EP1_MAXPACKETSIZE-2;
  src+=4;//ǰ�����ֽ��ǰ����������ݵĳ��� �ӵ������ֽڿ�ʼ��ȡ
  while(packet_length)
  {
    //��һ���ֽ������fifo
    usb_recv_fifo[usb_recv_fifo_in++]=*src++;
    if(usb_recv_fifo_in>=USB_FIFO_SIZE)usb_recv_fifo_in=0;
    packet_length--;
    if(packet_length)
    {
      //�ڶ����ֽ������fifo
      usb_recv_fifo[usb_recv_fifo_in++]=*src++;
      if(usb_recv_fifo_in>=USB_FIFO_SIZE)usb_recv_fifo_in=0;
      packet_length--;
    }
    //��Ϊ����Ӳ��fifo��32λ������ ����32λ��ǰ�����ֽ���Ч ���������������ֽ�
    src+=2;
  }
  SetEPRxStatus(ENDP1, EP_RX_VALID);
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

void EP1_IN_Callback(void)
{
 // count_in = 0;
 // UserToPMABufferCopy(Mouse_Buffer, GetEPTxAddr(ENDP1), 4);
  /* enable endpoint for transmission */
 // SetEPTxValid(ENDP1);
  if(usb_send_fifo_out==usb_send_fifo_in)
  {
    is_sending=0;
    usb_send_fifo_out = 0;
    usb_send_fifo_in = 0;
  }
  //������Ҫ���� ��Ӳ��FIFOװ��
  else
  {
    uint16_t *dst,*packet_length;
    uint32_t i, temp1, temp2;
    is_sending=1;
    dst=(uint16_t *)(ENDP1_TXADDR * 2 + PMAAddr);
    packet_length = dst;
    *packet_length = 0;
    dst ++;
    dst ++;
    for(i = 4; i != 0; i--)
    {
       if(usb_send_fifo_out !=  usb_send_fifo_in)
       {
           temp1 = (uint16_t) usb_send_fifo[usb_send_fifo_out];
           if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
              (*packet_length) ++;
        }
        else
       {
           temp1 = 0;
       }
       if(usb_send_fifo_out !=  usb_send_fifo_in)
       {
           temp2 = temp1 | (uint16_t)usb_send_fifo[usb_send_fifo_out] << 8;
           if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
           (*packet_length) ++;
        }
        else
        {
           temp2 = temp1;
        }
        *dst ++ = temp2;
        dst ++;
    }
    SetEPTxValid(ENDP1);
  }
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

//***********************************************************************************************************************
//��������:��ѯusb�Ľ���fifo ��������ݶ�ȡһ���ֽ�
//����˵��:ch---��ȡ�ַ���ŵ�ַ
//ע������:
//����˵��:��������ݷ���1
//***********************************************************************************************************************
uint8_t usb_read_char(uint8_t *ch)
{
	if(usb_recv_fifo_out==usb_recv_fifo_in)return 0;
	*ch=usb_recv_fifo[usb_recv_fifo_out];//�õ���ǰҪ��ȡ���ַ�
	usb_recv_fifo_out++;//ָ����һ��Ҫ��ȡ���ַ�
	if(usb_recv_fifo_out==USB_FIFO_SIZE)usb_recv_fifo_out=0;//�������fifoĩβ ���ش�ͷ��ʼ
	return 1;
}

//***********************************************************************************************************************
//��������:��usb���ݶ˵㷢��һ���ֽ�
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************

void usb_write_char(uint8_t ch)
{	
	uint16_t fifo_in_test;
	//���Ի������Ƿ���
        if(pInformation->Current_Configuration == 0)
        return;
	fifo_in_test=usb_send_fifo_in;
	fifo_in_test++;
	if(fifo_in_test>=USB_FIFO_SIZE)fifo_in_test=0;
	if(fifo_in_test==usb_send_fifo_out)return;
	//��FIFO�������
	usb_send_fifo[usb_send_fifo_in++]=ch;
	if(usb_send_fifo_in>=USB_FIFO_SIZE)usb_send_fifo_in=0;
		
	//����һ�������ж� �����жϷ���ʣ������
	if(is_sending==0)
	{
                  uint16_t *dst,*packet_length;
                  uint32_t i, temp1, temp2;
                  is_sending=1;
                  dst=(uint16_t *)(ENDP1_TXADDR * 2 + PMAAddr);
                  packet_length = dst;
                  *packet_length = 0;
                  dst ++;
                  dst ++;
                  for(i = 4; i != 0; i--)
                  {
                    if(usb_send_fifo_out !=  usb_send_fifo_in)
                    {
                      temp1 = (uint16_t) usb_send_fifo[usb_send_fifo_out];
                      if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                      (*packet_length) ++;
                    }
                    else
                    {
                      temp1 = 0;
                    }
                    if(usb_send_fifo_out !=  usb_send_fifo_in)
                    {
               //         if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                        temp2 = temp1 | (uint16_t)usb_send_fifo[usb_send_fifo_out] << 8;
                        if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                        (*packet_length) ++;
                    }
                    else
                    {
                       temp2 = temp1;
                    }
                    *dst ++ = temp2;
                    dst ++;
                  }
                  SetEPTxValid(ENDP1);
	}	
}

//***********************************************************************************************************************
//��������:��usb���ݶ˵㷢�Ͷ�������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void usb_write_buf(uint8_t *buf, uint16_t length)
{	

	uint16_t send_fifo_in_backup=usb_send_fifo_in;
        if(pInformation->Current_Configuration == 0)
        return;
	while(length--)
	{
		//���Ի������Ƿ���
		uint16_t fifo_in_test=usb_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=USB_FIFO_SIZE)fifo_in_test=0;
		if(fifo_in_test==usb_send_fifo_out)
		{
			//���������ָ�����֮ǰ��ָ��
			usb_send_fifo_in=send_fifo_in_backup;
			return;
		}
		//��FIFO�������
		usb_send_fifo[usb_send_fifo_in++]=*buf++;
		if(usb_send_fifo_in>=USB_FIFO_SIZE)usb_send_fifo_in=0;
	}

	//����һ�������ж� �����жϷ���ʣ������
	if(is_sending==0)
	{
                  uint16_t *dst,*packet_length;
                  uint32_t i, temp1, temp2;
                  is_sending=1;
                  dst=(uint16_t *)(ENDP1_TXADDR * 2 + PMAAddr);
                  packet_length = dst;
                  *packet_length = 0;
                  dst ++;
                  dst ++;
                  for(i = 4; i != 0; i--)
                  {
                    if(usb_send_fifo_out !=  usb_send_fifo_in)
                    {
                      temp1 = (uint16_t) usb_send_fifo[usb_send_fifo_out];
                      if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                      (*packet_length) ++;
                    }
                    else
                    {
                      temp1 = 0;
                    }
                    if(usb_send_fifo_out !=  usb_send_fifo_in)
                    {
               //         if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                        temp2 = temp1 | (uint16_t)usb_send_fifo[usb_send_fifo_out] << 8;
                        if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                        (*packet_length) ++;
                    }
                    else
                    {
                       temp2 = temp1;
                    }
                    *dst ++ = temp2;
                    dst ++;
                  }
                  SetEPTxValid(ENDP1);
	}
}

//***********************************************************************************************************************
//��������:��usb���ݶ˵㷢���ַ���
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void usb_write_str(char *str)
{
        if(pInformation->Current_Configuration == 0)
        return;
	while(*str)
	{
		//���Ի������Ƿ���
		uint16_t fifo_in_test=usb_send_fifo_in;
		fifo_in_test++;
		if(fifo_in_test>=USB_FIFO_SIZE)fifo_in_test=0;
		if(fifo_in_test==usb_send_fifo_out)return;
		//��FIFO�������
		usb_send_fifo[usb_send_fifo_in++]=*str++;
		if(usb_send_fifo_in>=USB_FIFO_SIZE)usb_send_fifo_in=0;
	}
	//����һ�������ж� �����жϷ���ʣ������
	if(is_sending==0)
	{
                  uint16_t *dst,*packet_length;
                  uint32_t i, temp1, temp2;
                  is_sending=1;
                  dst=(uint16_t *)(ENDP1_TXADDR * 2 + PMAAddr);
                  packet_length = dst;
                  *packet_length = 0;
                  dst ++;
                  dst ++;
                  for(i = 4; i != 0; i--)
                  {
                    if(usb_send_fifo_out !=  usb_send_fifo_in)
                    {
                      temp1 = (uint16_t) usb_send_fifo[usb_send_fifo_out];
                      if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                      (*packet_length) ++;
                    }
                    else
                    {
                      temp1 = 0;
                    }
                    if(usb_send_fifo_out !=  usb_send_fifo_in)
                    {
               //         if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                        temp2 = temp1 | (uint16_t)usb_send_fifo[usb_send_fifo_out] << 8;
                        if(++usb_send_fifo_out>=USB_FIFO_SIZE)usb_send_fifo_out=0;
                        (*packet_length) ++;
                    }
                    else
                    {
                       temp2 = temp1;
                    }
                    *dst ++ = temp2;
                    dst ++;
                  }
                  SetEPTxValid(ENDP1);
	}
}

