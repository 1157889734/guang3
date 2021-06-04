#include "tba_3093.h"
#include "main.h"

uint8_t ats2_connect_flag = 0;   //��ats���ӱ�ʶ��1:connected
uint32_t ats2_timer = 0;
uint8_t rx_422_2_buf[128];
void ats2_resp_cmd(uint8_t cmd, uint8_t serial_num);
void prc_422_2_normal(uint8_t tmp);
void prc_422_2_iscs(uint8_t ch);
void prc_422_2_clock(uint8_t tmp);
void prc_422_2_ats(uint8_t ch);
uint8_t get_connect_ats_2(void);

/****************************************************
*��������proc_422_2_data			       		         
*���ߣ�hk             							 
*������422�ӿ�2���ݴ���
*���룺��										
*�������								
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void proc_422_2_data()
{
    //��ats��������
	unsigned char tmp;
    if((ATS_MODE == com422_2_work_mode) && get_connect_ats_2() && check_timer(&ats2_timer, 1000))
    {
        ats2_resp_cmd(ATS_ACK_CMD, (tx_seq << 4) + (rx_seq & 0x0f));
    }
    
	
    //���û�����ݣ��ͷ���
    if(READ_CHAR_422_2(&tmp) == 0)
    {
        return;
    }
	switch(com422_2_work_mode)
	{
		case NORMAL:
			prc_422_2_normal(tmp);
		break;
		case ISCS_MODE:
			prc_422_2_iscs(tmp);
		break;
		case CLOCK:
			prc_422_2_clock(tmp);
		break;
		case GONGFANG:
			prc_422_2_normal(tmp);
		break;
  		case ATS_MODE:
        		 prc_422_2_ats(tmp);
         	break;
       }
}


/****************************************************
*��������prc_422_2_normal			       		         
*���ߣ�hk             							 
*��������ͨģʽ��ת��422�ӿ�2�����ݸ����������
*���룺tmp---����									
*�������								
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void prc_422_2_normal(uint8_t tmp)
{
	static uint8_t  index = 0; 
	static uint8_t  len = 0;
	static uint8_t  chksum;
      static uint8_t normal_stat = COM_RX_HEAD;
    
	switch(normal_stat)
	{
	case COM_RX_HEAD:
		if(tmp == 0x7e)
		{
			chksum = 0;
			index = 0;
			rx_422_2_buf[index++] = tmp;			
			normal_stat = COM_RX_LEN;
		}
		else
		{
			normal_stat = COM_RX_HEAD;
		}
		break;

	case COM_RX_LEN:
		if(tmp < 3)
		{
			normal_stat = COM_RX_HEAD;
		}
		else
		{
			len = tmp;
			rx_422_2_buf[index++] = tmp;
			normal_stat = COM_RX_PARA;		
		}		
		break;
	
	case COM_RX_PARA:
		rx_422_2_buf[index++] = tmp;
		chksum += tmp;
		if(--len > 0)
			normal_stat = COM_RX_PARA;
		else
			normal_stat = COM_RX_CHKSUM;
		break;

	case COM_RX_CHKSUM:
		rx_422_2_buf[index++] = tmp;
		if(tmp == chksum)
		{   //��ʲô�ͷ�ʲô������ת��
	             send_com_fifo(rx_422_2_buf, index);
			fill_sd_data_fifo(rx_422_2_buf, index, DATA_IN, 3);
		}
		normal_stat = COM_RX_HEAD;
		break;
	}
}


/****************************************************
*��������prc_422_2_iscs			       		         
*���ߣ�hk             							 
*�������ۺϼ�����ݴ���
*���룺ch---����									
*�������								
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void prc_422_2_iscs(uint8_t ch)
{
    static uint8_t iscs_stat = ISCS_SLAVE_ADDR;
    static uint8_t buf[10];
    static uint8_t index = 0;
    static uint16_t crc;

    switch(iscs_stat)
    {
        case ISCS_SLAVE_ADDR:  //�����жϴӻ���ַ
            if(ch == ISCS_ADDR)
            {
                index = 0;
                buf[index++] = ch;
                iscs_stat = ISCS_FUNCTION_CODE;
            }
        break;
            
        case ISCS_FUNCTION_CODE:
            buf[index++] = ch;
            iscs_stat = ISCS_REG_ADDR_H;
        break;
            
        case ISCS_REG_ADDR_H:
            buf[index++] = ch;
            iscs_stat = ISCS_REG_ADDR_L;
        break;
            
        case ISCS_REG_ADDR_L:
            buf[index++] = ch;
            iscs_stat = ISCS_VALUE_H;
        break;
            
        case ISCS_VALUE_H:
            buf[index++] = ch;
            iscs_stat = ISCS_VALUE_L;
        break;
            
        case ISCS_VALUE_L:
            buf[index++] = ch;
            iscs_stat = ISCS_CRC_L;
        break;

        case ISCS_CRC_L:
            crc = ch;
            iscs_stat = ISCS_CRC_H;
        break;

        case ISCS_CRC_H:
            crc += (uint16_t)ch << 8;
            if(crc == check_modbus_crc(buf, 6))
            {
                if(buf[ISCS_CMD_OFFS] == 0x06)
                {
                    //ת�����ݸ����������
                    send_to_com(0xe0, 0x01, 0, 1, &buf[ISCS_DATA_OFFS]);  
			 fas_flag=1;
			 fas_timer = get_time();
                }

            }
            iscs_stat = ISCS_SLAVE_ADDR;
        break;
            
        default:
        break;
    }
}


/****************************************************
*��������prc_422_2_clock			       		         
*���ߣ�hk             							 
*������ʱ�����ݴ���
*���룺tmp---����									
*�������								
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void prc_422_2_clock(uint8_t tmp)
{
	static unsigned char  clk_len = 0;
	static unsigned char checksum = 0;
      static uint8_t clock_stat = CLOCK_RX_STX;
    
	switch(clock_stat)
	{
	case CLOCK_RX_STX:
		if(tmp == CLOCK_HEAD1)
		{
			clk_len = 0;
			clock_stat = CLOCK_RX_STX1;
			checksum = tmp;
		}
		else
		{
			clock_stat = CLOCK_RX_STX;			
		}
		break;
		
	case CLOCK_RX_STX1:
		if(tmp == CLOCK_HEAD2)
		{
			clk_len = 0;
			clock_stat = CLOCK_RX_DATA;
			checksum += tmp;
		}
		else if(tmp == CLOCK_HEAD1)
		{
			clk_len = 0;
			clock_stat = CLOCK_RX_STX1;	//���ֵ�ǰ״̬����
			checksum = tmp;
		}else
		{
			clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
		}
		break;
		
	case CLOCK_RX_DATA:
		switch(clk_len)
		{
		case 0:
			if((tmp==0x41)||(tmp==0x47))
			{
				rx_422_2_buf[clk_len++] = tmp;
				checksum+=tmp;
			}
			else
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			break;
		case 1://n1
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;

		case 2://n2
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;

		case 3://n3
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;

		case 4://n4
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;

		case 5://m1
			if(tmp > '1')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;

		case 6://m2
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;

		case 7://D1
			if(tmp > '3')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		case 8://d2
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		case 9://w
			if(tmp > '6')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}			
			break;
		case 10://h1
			if(tmp > '2')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		case 11://h2
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		case 12://min1
			if(tmp > '5')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		case 13://min2
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		case 14://sec1
			if(tmp > '5')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
			else
			{
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		case 15://sec2
			if(tmp > '9')
			{
				clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			}
		      else
			{
				clock_stat = CLOCK_RX_ETX;	//ȥУ��״̬
				rx_422_2_buf[clk_len++] = tmp - '0';
				checksum += tmp;
			}
			break;
		default:
			clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
			break;
		
		}
		break;

	case CLOCK_RX_ETX:
		if(tmp == checksum)
		{
			prc_clock_data(rx_422_2_buf + 1);	//rx_422_2_buf[1]�������У׼ʱ���־�����ﲻ��Ҫ
		}
		clock_stat = CLOCK_RX_STX;	//ֱ�ӻص���ʼ״̬���������س����ͽ�����
		break;
		
	default:
		clock_stat = CLOCK_RX_STX;	//�س�ʼ״̬
		break;
	}
}

/****************************************************
*��������prc_422_2_ats			       		         
*���ߣ�hk             							 
*������ats���ݴ���
*���룺ch---����									
*�������								
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void prc_422_2_ats(uint8_t ch)
{
    static uint8_t  transcode_flag = 0;  //��0����ʾ֮ǰ����ת���ַ�
    static uint16_t index = 0;
    static uint16_t len = 0;
    static uint8_t ats_stat = ATS_HEAD_STAT;
    //uint32_t c_time=0,a_time=0;
    uint16_t crc;
    uint8_t i;
    uint8_t dat[20];
    uint16_t station_base;    //��¼ÿһվ��Ϣ�Ļ���ַ
    
    if(transcode_flag == 0 && ch != ATS_TRANSCODE)  //��ʾ��ǰ�ַ�����ͨ�ַ�
    {
        //���账��������ǰ��Ϊ�˼��ٳ�������ʱ�жϵĴ���
    }
    else if(transcode_flag && ch != ATS_TRANSCODE)   //���������ʾ��һ�ַ��ǿ�ʼ�ַ�
    {
        ats_stat = ATS_TARGET_ADDR_STAT; //ת�����Ŀ���ַ״̬
        transcode_flag = 0;
        index = 0;
        len = 0;
    }
    else if(transcode_flag && ch == ATS_TRANSCODE) //��ʾ��һ�ַ���ת���ַ�
    {
        transcode_flag = 0;
    }
    else   //���һ��������Ǳ�ʾ��ǰ�ַ���ת���ַ�
    {
        transcode_flag = 1;
        return;
    }
    
    //����¼��
    switch(ats_stat)
    {
        case ATS_HEAD_STAT:
        break;
        
        case ATS_TARGET_ADDR_STAT:
            ATS[ATS_TARGET_BASE] = ch;
            if(ch == ATS_BS_ADDR)
            {
                ats_stat = ATS_SOURCE_ADDR_STAT;
            }
		else   //�����ַ���ԣ���������
            {
                ats_stat = ATS_HEAD_STAT;
            }
        break;
        
        case ATS_SOURCE_ADDR_STAT:
            ATS[ATS_SOURCE_BASE] = ch;
            if(ch == ATS_ATS_ADDR)
            {
                ats_stat = ATS_SERIAL_NUM_STAT;           
            }
		else   //�����ַ���ԣ���������
            {
                ats_stat = ATS_HEAD_STAT;
            }
        break;
        
        case ATS_SERIAL_NUM_STAT:
            ATS[ATS_SERIAL_BASE] = ch;
            ats_stat = ATS_CONTROL_STAT;
        break;
        
        case ATS_CONTROL_STAT:
            ATS[ATS_CONTROL_BASE] = ch;
            switch(ch)  //���ݲ�ͬ�Ŀ������в�ͬ�����ݸ�ʽ
            {
                case ATS_CD2_CMD:
                case ATS_ACK_CMD:
                case ATS_NACK_CMD:  //ֱ��ȥ����У������
                    ats_stat = ATS_CRC_L_STAT;
                break;
                
                case ATS_STX_CMD:   //ȥ��������
                    ats_stat = ATS_DATA_LEN1_STAT;
                break;

                default:    //�ص�����״̬
                    ats_stat = ATS_HEAD_STAT;
                break;
            }
        break;
        
        case ATS_DATA_LEN1_STAT:
            ATS[ATS_DATA_BASE + index++] = ch;
            ats_stat = ATS_DATA_LEN2_STAT;
        break;

        case ATS_DATA_LEN2_STAT:
            ATS[ATS_DATA_BASE + index++] = ch;
            len = ((uint16_t)ATS[ATS_DATA_BASE + LENGTH_H_OFFS] << 8) + ATS[ATS_DATA_BASE + LENGTH_L_OFFS];
            if(len > ATS_DATA_MAX)  //����
            {
                ats_stat = ATS_HEAD_STAT;
            }
		else
            {
                ats_stat = ATS_DATA_OTHER_STAT;                
            }
        break;

        case ATS_DATA_OTHER_STAT:
            if(index < len + 1) //+����ΪҪ�ѡ�length���������ֽ�Ҳ���Data domain������
            {
                ATS[ATS_DATA_BASE + index++] = ch;
            }
	      else   //index == len+1 �����һ���ֽ�����
            {
                ATS[ATS_DATA_BASE + index++] = ch;
                ats_stat = ATS_CRC_L_STAT;
            }
        break;
        
        case ATS_CRC_L_STAT:
            ATS[ATS_CRC_L_BASE] = ch;
            ats_stat = ATS_CRC_H_STAT;
        break;
        
        case ATS_CRC_H_STAT:
            ATS[ATS_CRC_H_BASE] = ch;
            if(ATS_STX_CMD == ATS[ATS_CONTROL_BASE])
            {
                crc = check_ats_crc(ATS, len + 6);
            }else
            {
                crc = check_ats_crc(ATS, 4);
            }
            fill_sd_data_fifo(ATS, ATS_DATA_BASE + index, DATA_IN, 3);
            if(crc == ((uint16_t)ATS[ATS_CRC_H_BASE] << 8) + ATS[ATS_CRC_L_BASE])   //У����ȷ
            {
                switch(ATS[ATS_CONTROL_BASE])    //���ݲ�ͬ�Ŀ�����������
                {
                    case ATS_CD2_CMD:   //����ͨ������
                        tx_seq = 0;     //�յ�CD2��������㷢�ͺͽ������
                        rx_seq = 0;
                        ats2_connect_flag = 1;
			     ats2_timer = get_time();
                        ats2_resp_cmd(ATS_CD3_CMD, (tx_seq << 4) + (rx_seq & 0x0f));
                    break;
                    
                    case ATS_ACK_CMD:
                        //���账��
                    break;
                    
                    case ATS_NACK_CMD:
                        ats2_timer = get_time();
                        ats2_resp_cmd(ATS_RESEND_CMD, (tx_seq << 4) + (rx_seq & 0x0f));  //�ط���һ֡����
                    break;
                    
                    case ATS_STX_CMD:
                        ats2_timer = get_time();
			     rx_seq = (ATS[ATS_SERIAL_BASE]>>4) & 0x0f;
                        ats2_resp_cmd(ATS_ACK_CMD, (tx_seq << 4) + (rx_seq & 0x0f));
                       
                        //ÿ�η���һ��վ����Ϣ
                        station_base = ATS_DATA_BASE;
                        for(i = 0; i < ATS[ATS_DATA_BASE + PLATFORM_CNT_OFFS]; i++)
                        {
                            if(ATS[station_base + VALIDITY_FIELD_OFFS] == 1)    //Ϊ1��ʾ�ö�������Ч
                            {
                                dat[0] = LINE_NUM;  //�ߺ�
                                dat[1] = ATS[station_base + STATION_ID_OFFS];  //��վ���
                                dat[2] = ATS[station_base + PLATFORM_ID_OFFS]; //վ̨���
                                 if(1 == ATS[station_base + DEPARTURE_STAT_OFFS]) //�г���վ
                                {
                                    dat[3] = 0x02;
                                    send_to_net(0xE2, 0x01, 0, 4, dat);     //����վ����Ϣ
                                }
					else if(1 == ATS[station_base + ARRIVAL_STAT_OFFS]) //�г���վ
                                {
                                    dat[3] = 0x01;
                                    send_to_net(0xE2, 0x01, 0, 4, dat);     //����վ����Ϣ
                                }
					else
                                {
                                    dat[3] = 0x03;
									
                                    dat[4] = ATS[station_base + CURRENT_TIME_OFFS + 3];
                                    dat[5] = ATS[station_base + CURRENT_TIME_OFFS + 2];
                                    dat[6] = ATS[station_base + CURRENT_TIME_OFFS + 1];
                                    dat[7] = ATS[station_base + CURRENT_TIME_OFFS];
                                    //c_time=(uint32_t)dat[4]<<24;
					    //c_time+=(uint32_t)dat[5] <<16;
					   // c_time+=(uint32_t)dat[6] <<8;
					    //c_time+=(uint32_t)dat[7] ;
					 
                                    dat[8] = ATS[station_base + ARRIVAL_TIME_OFFS + 3];
                                    dat[9] = ATS[station_base + ARRIVAL_TIME_OFFS + 2];
                                    dat[10] = ATS[station_base + ARRIVAL_TIME_OFFS + 1];
                                    dat[11] = ATS[station_base + ARRIVAL_TIME_OFFS];

					    //a_time=(uint32_t)dat[8]<<24;
					    //a_time+=(uint32_t)dat[9] <<16;
					    //a_time+=(uint32_t)dat[10] <<8;
					    //a_time+=(uint32_t)dat[11] ;

                                    send_to_net(0xE2, 0x01, 0, 12, dat);     //����վ����Ϣ
                                }
                            }
                            station_base += STATION_DATA_LEN;   //�޸Ļ�ַ��ָ����һ����վ������
                        }
                    break;

                    default:
                    break;
                }
		
            }
	     else
            {
                ats2_timer = get_time();
                ats2_resp_cmd(ATS_NACK_CMD, (tx_seq << 4) + (rx_seq & 0x0f));    //��ʾУ�����
            }
            ats_stat = ATS_HEAD_STAT;
        break;
        
        default:
        break;
    }
}

/****************************************************
*��������ats2_resp_cmd			       		         
*���ߣ�hk             							 
*�������ظ�ATS�����ֻʵ�ֺܼ򵥵�Ӧ����
*���룺cmd---������		 serial_num	��ָ�ظ����ݵ���ţ���4λ�Ƿ�����ţ���4λ�ǽ������						
*�������								
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void ats2_resp_cmd(uint8_t cmd, uint8_t serial_num)
{
    static uint8_t  old_cmd = 0;
    uint8_t buf[10];
    uint8_t index;
    uint16_t crc;
    uint8_t i;

    index = 0;
    buf[index++] = ATS_TRANSCODE;
    buf[index++] = ATS_ATS_ADDR;
    buf[index++] = ATS_BS_ADDR;
    buf[index++] = serial_num;

    if(ATS_RESEND_CMD == cmd && 0 != old_cmd)   //������ط�������ط���һ֡����
    {
        buf[index++] = old_cmd;
    }else
    {
        buf[index++] = cmd;
        old_cmd = cmd;
    }
    crc = check_ats_crc(buf+1, index-1);
    buf[index++] = crc & 0x00ff;
    buf[index++] = crc >> 8;

    SEND_CHAR_422_2(ATS_TRANSCODE);          //������ʼ�ַ�
    for(i = 1; i<index; i++)
    {
        if(buf[i] == ATS_TRANSCODE)
        {
            SEND_CHAR_422_2(ATS_TRANSCODE);  //����ת���ַ�
        }
        SEND_CHAR_422_2(buf[i]);
    }
}

/****************************************************
*��������get_connect_ats_2			       		         
*���ߣ�hk             							 
*����������422�ӿ�2��ats������״̬
*���룺��								
*�������								
*����ֵ��1:connected
*�޸ļ�¼����								
****************************************************/
uint8_t get_connect_ats_2()
{
    return ats2_connect_flag;

}
