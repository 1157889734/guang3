//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ͨ�ú����� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:utils.c

//*�ļ�����:ͨ�ú�����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"



//***********************************************************************************************************************
//��������:16λ�ߵ��ֽڽ���
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
uint16 swap_int16(uint16 n)
{
	uint16 temp1,temp2;
	
	temp1=(n>>8)&0x00FF;
	temp2=(n<<8)&0xFF00;
	return temp1|temp2;
}

//***********************************************************************************************************************
//*��������:������ת����ʮ���ƴ�
//*����˵��:dec---ת���ɵ�ʮ���ƴ� n--Ҫת��������
//*ע������:
//*����˵��:��
//***********************************************************************************************************************
uint8 int_to_dec(uint8 *dec, uint32 n)
{	
	uint8 i=0;
	uint8 result;
	uint32 divisor, remainder;

	remainder=n;
	if(n<0x10000)divisor=10000;
	else divisor=1000000000;
	for(;divisor>0;divisor/=10)
	{
		result=remainder/divisor;
		remainder%=divisor;
		if(result!=0 || divisor==1 || i>0)
		dec[i++]=result+0x30;
	}
	dec[i]=0;
	
	return i;//��������λ��
}

//***********************************************************************************************************************
//��������:�ѻ���������ת����Ϊhex�ַ���
//����˵��:buf
//ע������:
//����˵��:�����ȷ���1
//***********************************************************************************************************************
void buf_to_hex(uint8 *hex, uint8 *buf, uint16 length)
{
	uint8 asiic[]="0123456789ABCDEF";
	while(length--)
	{
		*hex++=asiic[(*buf)>>4];
		*hex++=asiic[(*buf++)&0x0F];
	}
	*hex=0;
}

//***********************************************************************************************************************
//*��������:������ת����hex��
//*����˵��:hex---ת���ɵ�hex�� n--Ҫת��������
//*ע������:
//*����˵��:
//***********************************************************************************************************************
void char_to_hex(uint8 *hex, uint8 n)
{
	uint8 asiic[]="0123456789ABCDEF";

	*hex++=asiic[n>>4];
	*hex++=asiic[n&0x0F];
	*hex=0;
}

//***********************************************************************************************************************
//*��������:������ת����hex��
//*����˵��:hex---ת���ɵ�hex�� n--Ҫת��������
//*ע������:
//*����˵��:
//***********************************************************************************************************************
void int_to_hex(uint8 *hex, uint32 n)
{
	uint8 asiic[]="0123456789ABCDEF";

	if(n<0x10000)
	{
		*hex++=asiic[(n>>12)&0x0F];
		*hex++=asiic[(n>>8 )&0x0F];
		*hex++=asiic[(n>>4 )&0x0F];
		*hex++=asiic[(n>>0 )&0x0F];
	}
	else
	{
		*hex++=asiic[(n>>28)&0x0F];
		*hex++=asiic[(n>>24)&0x0F];
		*hex++=asiic[(n>>20)&0x0F];
		*hex++=asiic[(n>>16)&0x0F];
		*hex++=asiic[(n>>12)&0x0F];
		*hex++=asiic[(n>>8 )&0x0F];
		*hex++=asiic[(n>>4 )&0x0F];
		*hex++=asiic[(n>>0 )&0x0F];	
	}
	*hex=0;
}

//***********************************************************************************************************************
//��������:��hex�ַ���--ת��Ϊ����
//����˵��:hex---�����Ĵ�
//ע������:
//����˵��:�����Ƿ�ɹ�
//***********************************************************************************************************************
static uint8 xdigitvalue(uint8 isdigit)//�ж��Ƿ���ʮ����������
{
	if(isdigit>='0' && isdigit<='9')
	{
		return isdigit-'0';
	}
	else if(isdigit>='A' && isdigit<='F')
	{
		return isdigit-'7';
	}
	else if (isdigit >= 'a' && isdigit <= 'f') 
	{
		return 10 + (isdigit - 'a');
	}
	else
	{
		return 0xFF;
	}
}
uint8 hex_to_int8(uint8 *hex, uint8 *n)
{
	char i,k;

	*n=0;
	for(k=0;k<2;k++)
	{
		if((i=xdigitvalue(hex[k]))==0xFF)return 0;
		*n=((*n)<<4)|i;
	}
	return 1;
}
uint8 hex_to_int16(uint8 *hex, uint16 *n)
{
	char i,k;

	*n=0;
	for(k=0;k<4;k++)
	{
		if((i=xdigitvalue(hex[k]))==-1)return 0;
		*n=((*n)<<4)|i;
	}
	return 1;
}
uint8 hex_to_int32(uint8 *hex, uint32 *n)
{
	char i,k;

	*n=0;
	for(k=0;k<8;k++)
	{
		if((i=xdigitvalue(hex[k]))==-1)return 0;
		*n=((*n)<<4)|i;
	}
	return 1;
}
uint16 hex_to_buf(uint8 *hex, uint8 *buf)
{
	uint16 length=0;
	
	while(*hex)
	{
		hex_to_int8(hex,buf++);
		hex+=2;
		length++;
	}
	*buf=0;
	return length;
}

//***********************************************************************************************************************
//��������:��ʮ�����ַ���ת��������
//����˵��:dec--Ҫת����ʮ���ƴ�  n--ת���ɵ�����
//ע������:
//����˵��:�����Ƿ�ɹ�
//***********************************************************************************************************************
static char digitvalue(uint8 isdigit)//�ж��Ƿ���ʮ��������
{
	if(isdigit>='0' && isdigit<='9')return isdigit-'0';
	return (char)-1;
}
uint8 dec_to_int8(uint8 *dec, uint8 *n)
{
	char i;

	*n=0;
	while(*dec)
	{
		if((i=digitvalue(*dec))==-1)return 0;	
		*n=((*n)*10)+i;
		dec++;
	}
	return 1;
}
uint8 dec_to_int16(uint8 *dec, uint16 *n)
{
	char i;

	*n=0;
	while(*dec)
	{
		if((i=digitvalue(*dec))==-1)return 0;	
		*n=((*n)*10)+i;
		dec++;
	}
	return 1;
}
uint8 dec_to_int32(uint8 *dec, uint32 *n)
{
	char i;

	*n=0;
	while(*dec)
	{
		if((i=digitvalue(*dec))==-1)return 0;	
		*n=((*n)*10)+i;
		dec++;
	}
	return 1;
}
/****************************************************
*��������print_line			       		         
*���ߣ�hk             							 
*��������ӡ������Ϣ
*���룺��      
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
void print_line(char *str)
{
	if((sh9_ptu_dev.print_flag==1)||(sh9_ptu_dev.print_flag==3))
	{
		DEBUG_SEND_STR("\r\n");
		DEBUG_SEND_STR(str);
		DEBUG_SEND_STR("\r\n");
	}
}

/****************************************************
*��������debug_print_line			       		         
*���ߣ�hk             							 
*��������ӡ������Ϣ
*���룺��      
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
void debug_print_line(char *str)
{
    #if DEBUG_PRINT
	print_line(str);
    #endif
}

/****************************************************
*��������print_int			       		         
*���ߣ�hk             							 
*��������ӡint ������Ϣ
*���룺��      
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
void print_int(char *args, uint32 n)
{
	uint8 buf[11];
	
	if(args[1]=='x')int_to_hex(buf,n);
	else if(args[1]=='d')int_to_dec(buf,n);
	DEBUG_SEND_STR(args+2);
	DEBUG_SEND_STR((char *)&buf);
       
	DEBUG_SEND_STR("\r\n");
}

/****************************************************
*��������debug_print_int			       		         
*���ߣ�hk             							 
*��������ӡint ������Ϣ
*���룺��      
*�������	
*����ֵ����	
*�޸ļ�¼����								
****************************************************/
void debug_print_int(char *args, uint32 n)
{
	#if DEBUG_PRINT
	    print_int(args, n);
	/* uint8 asiic[]="0123456789ABCDEF";
	uint8 buf[11];
	buf[0]=asiic[(n>>4 )&0x0F];
	buf[1]=asiic[(n>>0 )&0x0F];
	DEBUG_SEND_STR(args+2);
	DEBUG_SEND_STR((char *)&buf);  */
	#endif
}

//***********************************************************************************************************************
//��������:��ӡ������
//����˵��:
//ע������:
//����˵��:��
//***********************************************************************************************************************
void print_buf(uint8 *buf, uint16 length)
{
	uint8 hex[11];
	
	while(length--)
	{
		int_to_hex(hex,*buf++);
		DEBUG_SEND_STR((char *)&hex);
	}
	DEBUG_SEND_STR("\r\n");
}

void ptu_print_buf(uint8 * buf, uint16 length)
{
	uint8 hex[11];

	while(length--)
	{
		int_to_hex(hex,*buf++);
		DEBUG_SEND_STR((char *)&hex+2);
		DEBUG_SEND_STR(" ");
	}
	DEBUG_SEND_STR("\r\n");
}

void ptu_print_tbabuf(uint8 * buf, uint16 length)
{
	uint8 hex[11];

	DEBUG_SEND_STR("7E");
	DEBUG_SEND_STR(" ");
	while(length--)
	{
		int_to_hex(hex,*buf++);
		DEBUG_SEND_STR((char *)&hex+2);
		DEBUG_SEND_STR(" ");
	}
	DEBUG_SEND_STR("7E");
	DEBUG_SEND_STR("\r\n");
	DEBUG_SEND_STR("\r\n");
}

void ptu_print_debug(char * str)
{
	if((sh9_ptu_dev.print_flag==2)||(sh9_ptu_dev.print_flag==3))
	{
		DEBUG_SEND_STR("\r\n");
		DEBUG_SEND_STR(str);
		DEBUG_SEND_STR("\r\n");
	}
		
}


