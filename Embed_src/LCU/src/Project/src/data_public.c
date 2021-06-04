/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: ���������õ���һЩ���������ŵ�����
**  ��������: 2013.12.19
**
**  ��ǰ�汾��1.0
**  ���ߣ����
*******************************************************************************
*/
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "uart4.h"
#include "utils.h"
#include "include.h"




/*
ִ��ת�벢�ж�У��
*/
u8 VerifyPaket(u8 *paketBuf, u16 length)
{
    u8 recv_flag=0;
    u8 checksum=0;
    u16 src_length=0;//��ת��֮ǰ�ĳ���
    u16 dst_length=0;//��ת���Ժ�ĳ���
//    static u8 length_of_send = 0;
    	    
           //ִ����ת�� ������У���
	   for(src_length=0; src_length<length; src_length++)
	    {
			//��������
			paketBuf[dst_length]=paketBuf[src_length];
			//�жϸոտ������Ƿ���ת���
			if(paketBuf[src_length]==TBA_PACKET_DLE && src_length<length-1)
			{
				//�ж���һ���ַ��Ƿ��Ƿָ����ת��� �Ѹոտ�����ת�����ԭΪTMS_PACKET_TAB ��������һ���ַ�
				if(paketBuf[src_length+1]==TBA_PACKET_DLE_TAB)
				{
					paketBuf[dst_length]=TBA_PACKET_TAB;//�Ѹոտ�����ת�����ԭΪTMS_PACKET_TAB
					src_length++;//��������һ���ַ�
				}
				//�ж���һ���ַ��Ƿ���ת�����ת��� �Ѹոտ�����ת�����ԭΪTMS_PACKET_DLE ��������һ���ַ�
				else if(paketBuf[src_length+1]==TBA_PACKET_DLE_DLE)
				{
					paketBuf[dst_length]=TBA_PACKET_DLE;//�Ѹոտ�����ת�����ԭΪTMS_PACKET_DLE
					src_length++;//��������һ���ַ�
				}
				else
				{
					//print_line("tms 7f error");
					//���ݴ���----7fû�б���ȷת��
					dst_length=0;//���ת���Ժ�ĳ���
					break;
				}
			} 
			checksum+=paketBuf[dst_length];
			dst_length++;
	    }
	   
		if(checksum == 0x55)
		{
			recv_flag = dst_length-1;  //����У��͵ĳ���
		}
		else
		{
			recv_flag = 0;
		}
		
		return recv_flag;
}



/*
����:  ʵ�����ݴ����
             1����Ӱ�ͷ7E
             2����Ӱ�β7E
             3������ת��
             4������У���
*/
u8 PackageData( u8 *destBuf, u8 *srcBuf, u8 length )
{
	u8 send_length = 0;
	u8 checksum=0;
	u8 cnt = 0;
	u8 *ptr = NULL;


	destBuf[send_length++] = 0x7e;

       ptr = (u8 *)srcBuf;
	for(cnt = 0; cnt < length; cnt++)
	{
	    if(*ptr == 0x7e)
	    {
	         destBuf[send_length++] = 0x7f;
		   destBuf[send_length++] = 0x80;
	    }
	    else if(*ptr == 0x7f)
	    {
	        destBuf[send_length++] = 0x7f;
		destBuf[send_length++] = 0x81;
	    }
	     else
	     {
	     	 destBuf[send_length++] = *ptr;
	     }
	     checksum += *ptr;
	     ptr++;
	}

	checksum = 0x55 -checksum;
      //У���
	if(checksum == 0x7e)
	{
	    destBuf[send_length++] = 0x7f;
          destBuf[send_length++] = 0x80;
	}
	else if(checksum == 0x7f)
	{
	    destBuf[send_length++] = 0x7f;
           destBuf[send_length++] = 0x81;
	}
      else
	{
	    destBuf[send_length++] = checksum;
	}
	//send_buf[send_length++] = checksum;  //У���
      destBuf[send_length++] = 0x7e;

	return send_length;
}



