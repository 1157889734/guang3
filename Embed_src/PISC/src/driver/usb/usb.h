//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& usb���� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:usb.h

//*�ļ�����:usb����

//*�ļ�����:��  ��

//*��������:2007��5��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef USB_H
#define USB_H



//�ж�״̬�Ĵ���״̬λ
#define ISTR_CTR   	(0x8000)	//Correct TRansfer	        (clear-only bit)
#define ISTR_DOVR  	(0x4000)	//DMA OVeR/underrun		(clear-only bit)
#define ISTR_ERR   	(0x2000)	//ERRor				(clear-only bit)
#define ISTR_WKUP  	(0x1000)	//WaKe UP			(clear-only bit)
#define ISTR_SUSP  	(0x0800)	//SUSPend			(clear-only bit)
#define ISTR_RESET 	(0x0400)	//RESET				(clear-only bit)
#define ISTR_SOF   	(0x0200)	//Start Of Frame		(clear-only bit)
#define ISTR_ESOF  	(0x0100)	//Expected Start Of Frame	(clear-only bit)
#define ISTR_DIR   	(0x0010)	//DIRection of transaction	(read-only bit) 
#define ISTR_EP_ID 	(0x000F)	//EndPoint IDentifier		(read-only bit) 

//���ƼĴ���״̬λ
#define CNTR_CTRM   (0x8000)	//Correct TRansfer Mask
#define CNTR_DOVRM  (0x4000)	//DMA OVeR/underrun Mask
#define CNTR_ERRM   (0x2000)	//ERRor Mask
#define CNTR_WKUPM  (0x1000)	//WaKe UP Mask
#define CNTR_SUSPM  (0x0800)	//SUSPend Mask
#define CNTR_RESETM (0x0400)	//RESET Mask  
#define CNTR_SOFM   (0x0200)	//Start Of Frame Mask
#define CNTR_ESOFM  (0x0100)	//Expected Start Of Frame Mask
#define CNTR_RESUME (0x0010)	//RESUME request
#define CNTR_FSUSP  (0x0008)	//Force SUSPend
#define CNTR_LPMODE (0x0004)	//Low-power MODE
#define CNTR_PDWN   (0x0002)	//Power DoWN
#define CNTR_FRES   (0x0001)	//Force USB RESet


//�������Ĵ���״̬λ
#define FNR_RXDP	(0x8000)	//status of D+ data line
#define FNR_RXDM	(0x4000)	//status of D- data line
#define FNR_LCK		(0x2000)	//LoCKed
#define FNR_LSOF	(0x1800)	//Lost SOF
#define FNR_FN		(0x07FF)	//Frame Number

//�豸��ַ�Ĵ���������
#define DADDR_EF	(0x80)
#define DADDR_ADD	(0x7F)

//�˵�Ĵ���״̬λ
#define EP_CTR_RX      (0x8000) //EndPoint Correct TRansfer RX
#define EP_DTOG_RX     (0x4000) //EndPoint Data TOGGLE RX
#define EPRX_STAT      (0x3000)	//EndPoint RX STATus bit field
#define EP_SETUP       (0x0800)	//EndPoint SETUP
#define EP_T_FIELD     (0x0600) //EndPoint TYPE
#define EP_KIND        (0x0100) //EndPoint KIND
#define EP_CTR_TX      (0x0080) //EndPoint Correct TRansfer TX
#define EP_DTOG_TX     (0x0040) //EndPoint Data TOGGLE TX
#define EPTX_STAT      (0x0030)	//EndPoint TX STATus bit field
#define EPADDR_FIELD   (0x000F) //EndPoint ADDRess FIELD

//�˵�Ĵ�������λ
#define EPREG_MASK     (EP_CTR_RX|EP_SETUP|EP_T_FIELD|EP_KIND|EP_CTR_TX|EPADDR_FIELD)

//�˵�������
#define EP_BULK        (0x0000)	//EndPoint BULK
#define EP_CONTROL     (0x0200) //EndPoint CONTROL
#define EP_ISOCHRONOUS (0x0400) //EndPoint ISOCHRONOUS
#define EP_INTERRUPT   (0x0600) //EndPoint INTERRUPT
#define EP_T_MASK      (~EP_T_FIELD & EPREG_MASK)

//EP_KIND EndPoint KIND
#define EPKIND_MASK    (~EP_KIND & EPREG_MASK)

//����״̬����
#define EP_TX_DIS      (0x0000)	//EndPoint TX DISabled
#define EP_TX_STALL    (0x0010)	//EndPoint TX STALLed
#define EP_TX_NAK      (0x0020) //EndPoint TX NAKed
#define EP_TX_VALID    (0x0030)	//EndPoint TX VALID
#define EPTX_DTOG1     (0x0010)	//EndPoint TX Data TOGgle bit1
#define EPTX_DTOG2     (0x0020)	//EndPoint TX Data TOGgle bit2
#define EPTX_DTOGMASK  (EPTX_STAT|EPREG_MASK)

//����״̬����
#define EP_RX_DIS      (0x0000)	//EndPoint RX DISabled
#define EP_RX_STALL    (0x1000)	//EndPoint RX STALLed
#define EP_RX_NAK      (0x2000)	//EndPoint RX NAKed
#define EP_RX_VALID    (0x3000)	//EndPoint RX VALID
#define EPRX_DTOG1     (0x1000)	//EndPoint RX Data TOGgle bit1
#define EPRX_DTOG2     (0x2000)	//EndPoint RX Data TOGgle bit1
#define EPRX_DTOGMASK  (EPRX_STAT|EPREG_MASK)



//����������
#define USB_FIFO_SIZE			512


//�жϿ������Ĳ�
#define USB_IRQ_SLOT			EIC_SIR26
//�ж�������
#define USB_IRQ_VECTOR			26
//�ж����ȼ�
#define USB_IRQ_PRIORITY		2	//���15


//�жϿ������Ĳ�
#define USB_DET_IRQ_SLOT		EIC_SIR5
//�ж�������
#define USB_DET_IRQ_VECTOR		5
//�ж����ȼ�
#define USB_DET_IRQ_PRIORITY		1	//���15



/* defines how many endpoints are used by the device */
#define TOTAL_ENDP		(2)
#define TOTAL_CONFIG_ENDP	(1)


/* --------------   Buffer Description Table  -----------------*/
/* buffer table base address */
#define BTABLE_ADDRESS      (0x10)

/* EP0 */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x80)
#define ENDP0_TXADDR        (0x90)

/* EP1 */
/* interrupt endpoint */
/* rx buffer base address */
#define ENDP1_TXADDR   		(0x100)
#define ENDP1_RXADDR   		(0x200)

#define STD_MAXPACKETSIZE	8		//��׼�������󳤶�
#define EP1_MAXPACKETSIZE	10		//�˵�1���ݰ���󳤶�


/* IMR_MSK */
#define IMR_MSK (CNTR_CTRM|CNTR_WKUPM|CNTR_SUSPM|CNTR_ERRM|CNTR_SOFM|CNTR_ESOFM|CNTR_RESETM)



extern void usb_init(uint32 bps);
extern uint8 usb_read_char(uint8 *ch);
extern void usb_write_char(uint8 ch);
extern uint8 usb_write_buf(uint8 *buf, uint16 length);
extern void usb_write_str(char *str);
extern uint8 usb_get_connect_state(void);



//���ý��ջ���������
extern void SetEPRxCount(uint8 bEpNum, uint16 wCount);
//���ý���״̬
extern void SetEPRxStatus(uint8 bEpNum, uint16 wState);
//���÷��Ͷ˵�״̬
extern void SetEPTxStatus(uint8 bEpNum, uint16 wState);
//�����豸��ַ
extern void SetDeviceAddress(uint8 Val);


#endif


