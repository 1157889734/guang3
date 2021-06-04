
/******************************************************************************
*�ļ����ƣ�error.c		       		         
*��          �ߣ�hk           							 
*��          �����豸������
*�������ڣ�2010-3-14
*�޸ļ�¼��hk   2010-5-21  for  �쳣���ݴ洢								
******************************************************************************/


#include "include.h"

#define  	ONE_ERROR_RECORD   56   //һ���洢��Ϣ��С(�ֽ�)


static uint32  current_addr = 0;
error_packet_t error_pack;
//error_packet_t pre_error_pack;
static uint8  error_record_flag = 0;   //��ȡ�쳣�洢��Ϣ�ı�ʶ 1:��ȡ�쳣�洢�� 0:��֮

/****************************************************
*��������	set_error_record 		         
*���ߣ�hk             							 
*���������û�ȡ�쳣�洢��Ϣ��ʶ
*���룺flag ---- 1  ��ȡ�쳣�洢�� 
*�����	��
*����ֵ����
*�޸ļ�¼����								
****************************************************/
void set_error_record(uint8 flag)
{
    error_record_flag = flag;
    //ÿ��ֻ��ȡ���500����¼
    if((saveparam.error_flash_addr - 0)>= ONE_ERROR_RECORD*500)
    	{
    	    current_addr = saveparam.error_flash_addr - ONE_ERROR_RECORD*500;
    	}
    else
    	{
          current_addr = 0;
    	}
}

/************************************************************************************************************************
��������:�˿ͽ����������ϴ���
����˵��:��
����: suven
����: 2012-12-20
*************************************************************************************************************************/
static void error_pecu_proc(void)
{
	return;
	#if 0
	sh9_tms_dev.tms_send_packet.dw3.PICU1_comerror_car1 = lcu_default_data[0].icc_error.pecu1;
	sh9_tms_dev.tms_send_packet.dw3.PICU2_comerror_car1 = lcu_default_data[0].icc_error.pecu2;
	sh9_tms_dev.tms_send_packet.dw3.PICU3_comerror_car1 = lcu_default_data[0].icc_error.pecu3;

	sh9_tms_dev.tms_send_packet.dw3.PICU1_comerror_car2 = lcu_default_data[1].icc_error.pecu1;
	sh9_tms_dev.tms_send_packet.dw3.PICU2_comerror_car2 = lcu_default_data[1].icc_error.pecu2;
	sh9_tms_dev.tms_send_packet.dw3.PICU3_comerror_car2 = lcu_default_data[1].icc_error.pecu3;

	sh9_tms_dev.tms_send_packet.dw3.PICU1_comerror_car3 = lcu_default_data[2].icc_error.pecu1;
	sh9_tms_dev.tms_send_packet.dw3.PICU2_comerror_car3 = lcu_default_data[2].icc_error.pecu2;
	sh9_tms_dev.tms_send_packet.dw3.PICU3_comerror_car3 = lcu_default_data[2].icc_error.pecu3;

	sh9_tms_dev.tms_send_packet.dw4.PICU1_comerror_car4 = lcu_default_data[3].icc_error.pecu1;
	sh9_tms_dev.tms_send_packet.dw4.PICU2_comerror_car4 = lcu_default_data[3].icc_error.pecu2;
	sh9_tms_dev.tms_send_packet.dw4.PICU3_comerror_car4 = lcu_default_data[3].icc_error.pecu3;

	sh9_tms_dev.tms_send_packet.dw4.PICU1_comerror_car5 = lcu_default_data[4].icc_error.pecu1;
	sh9_tms_dev.tms_send_packet.dw4.PICU2_comerror_car5 = lcu_default_data[4].icc_error.pecu2;
	sh9_tms_dev.tms_send_packet.dw4.PICU3_comerror_car5 = lcu_default_data[4].icc_error.pecu3;

	sh9_tms_dev.tms_send_packet.dw4.PICU1_comerror_car6 = lcu_default_data[5].icc_error.pecu1;
	sh9_tms_dev.tms_send_packet.dw4.PICU2_comerror_car6 = lcu_default_data[5].icc_error.pecu2;
	sh9_tms_dev.tms_send_packet.dw4.PICU3_comerror_car6 = lcu_default_data[5].icc_error.pecu3;
	#endif
}



/****************************************************
*��������	print_error_info 		         
*���ߣ�hk             							 
*����������쳣��Ϣ
*���룺��
*�����	��
*����ֵ����
*�޸ļ�¼����								
****************************************************/
static void print_error_info(void)
{
    
    uint32  end_addr = 0;
    uint8   buf[ONE_ERROR_RECORD*2] = {0};
    uint16 length = 0;
    static uint32  check_time = 0;

    uint32 addr_tmp0;
     uint16 addr_temp1;
	
   //ptu ��ѯ�쳣��Ϣ
    if(error_record_flag &&(checktimer(&check_time, 40)))
    {
    //print_line("read error info");
		 
        if(saveparam.re_write_flash)
		end_addr = Endaddr;
	 else
	 	end_addr = saveparam.error_flash_addr;

	 if((end_addr - current_addr)>= ONE_ERROR_RECORD)
	 {
	     length = ONE_ERROR_RECORD;
	 }
	 else
	 {
	     length = (end_addr - current_addr);
	     //ptu ��ѯ�쳣��Ϣλ��0
	 }

	 if(length ==0 )//�Ѿ�����β��
	 {
	     current_addr = 0;
	     error_record_flag = 0;
	     buf[0] = SETUP_CMD_DIS_ERROR;   //���ͽ������ݰ�
	     buf[1] = 0x00;
	     buf[2] = 0x00;
	     ptu_dev->ptu_send_data(buf, 3);
	 }
	 else
	 {
	     addr_temp1 = current_addr%PAGE_SIZE;
	     addr_tmp0 = current_addr;
	     if((PAGE_SIZE-addr_temp1) < ONE_ERROR_RECORD)
	     	{
	     	    read_flash(addr_tmp0, &buf[3], (PAGE_SIZE-addr_temp1));
		    addr_tmp0 = addr_tmp0 + (PAGE_SIZE-addr_temp1);
		    read_flash(addr_tmp0, &buf[3+(PAGE_SIZE-addr_temp1)], ONE_ERROR_RECORD-(PAGE_SIZE-addr_temp1));
	     	}
	     else
	     	{
	          read_flash(current_addr, &buf[3], length);
	     	}
	     current_addr += length;
	    // print_buf(buf, length);
	    // PTU_SEND_CHAR(0x7e);
	     buf[0] = SETUP_CMD_DIS_ERROR;
	     buf[1] = 0x00;
	     buf[2] = length;
	     //if(buf[37] != 0xff)//��ܶ�дflash�쳣�����--��14�����ݵĺ�20���ֽ�ȫ��0xff
	     {
	         ptu_dev->ptu_send_data(buf, length+3);
		  //uart0_write_buf(buf,length+3);
	     	}
	    // PTU_SEND_CHAR(0x7e);
	 }
    }
}

static void save_error_info(void)
{
	//uint8 index = 0;
	static uint32  check_time = 0;
	//uint8 dev_type_tmp= 0;
	//uint8 dev_no_tmp = 0;
	uint8 lcu_cnt = 0;
	uint8 buf[ONE_ERROR_RECORD] = {0};
	uint8 write_buf[PAGE_SIZE] = {0};
	uint8 rtc_time[5] = {0};
	static uint8 num = 0;

	uint32 addr_tmp0;
	uint16 addr_temp1;

	/*
	for(index = 0; index < POLL_NUM_MAX; index++)
	{
		if(token_polling_table[index].poll_error >= 10)     //Ѳ�����س�ʱ
		{
			dev_type_tmp = token_polling_table[index].dev_id&0x0f;
		    	dev_no_tmp = token_polling_table[index].dev_id >> 4;
		    	if(dev_type_tmp == 1)//pisc
		    	{
		        	error_pack.status1 |= (1<<(dev_no_tmp+5));
		    	}
		    	else if(dev_type_tmp == 2) //dcp
		    	{
			    	error_pack.status2 |= (1<<(dev_no_tmp+4));
		    	}
		    	else if(dev_type_tmp == 3)//lcu
			{
			    	error_pack.status1 |= (1<<(dev_no_tmp-1));
			}
		}
	}
	*/
	//dvaͨѶ����
       if(sh9_dva_dev.comm_error > 70)    
       {
        	if(read_pisc_id() == 1)
            		error_pack.status2 |= (1<<0);   //dva1
		else
			error_pack.status2 |= (1<<1);	//dva2
       }
       else
    	{
		if(read_pisc_id() == 1)
			error_pack.status2 &= ~(1<<0);
		else
			error_pack.status2 &= ~(1<<1);
    	}

	if(other_default_data.cab_status.dva_error)
	{
		if(read_pisc_id() == 2)
            		error_pack.status2 |= (1<<0);   //dva1
		else
			error_pack.status2 |= (1<<1);	//dva2
	}
	 else
    	{
		if(read_pisc_id() == 2)
			error_pack.status2 &= ~(1<<0);
		else
			error_pack.status2 &= ~(1<<1);
    	}

	//apuͨѶ����
       if(my_default_data.cab_status.apu_error)    
       {
        	if(read_pisc_id() == 1)
            		error_pack.status2 |= (1<<2);  
		else
			error_pack.status2 |= (1<<3);	
       }
       else
    	{
		if(read_pisc_id() == 1)
			error_pack.status2 &= ~(1<<2);
		else
			error_pack.status2 &= ~(1<<3);
    	}

	if(other_default_data.cab_status.apu_error)
	{
		if(read_pisc_id() == 2)
            		error_pack.status2 |= (1<<2);   
		else
			error_pack.status2 |= (1<<3);	
	}
	 else
    	{
		if(read_pisc_id() == 2)
			error_pack.status2 &= ~(1<<2);
		else
			error_pack.status2 &= ~(1<<3);
    	}

	//recordͨѶ����
       if(my_default_data.cab_status.record_error)    
       {
        	if(read_pisc_id() == 1)
            		error_pack.status2 |= (1<<6);  
		else
			error_pack.status2 |= (1<<7);	
       }
       else
    	{
		if(read_pisc_id() == 1)
			error_pack.status2 &= ~(1<<6);
		else
			error_pack.status2 &= ~(1<<7);
    	}

	if(other_default_data.cab_status.record_error)
	{
		if(read_pisc_id() == 2)
            		error_pack.status2 |= (1<<6);   
		else
			error_pack.status2 |= (1<<7);	
	}
	 else
    	{
		if(read_pisc_id() == 2)
			error_pack.status2 &= ~(1<<6);
		else
			error_pack.status2 &= ~(1<<7);
    	}

	//atc ����
	if(my_default_data.tms_status.tms_valid)
	{
		if(read_pisc_id() == 1)
			error_pack.status3 |= (1<<0);   
		else
			error_pack.status3 |= (1<<1);   
	}
	else
    	{
		if(read_pisc_id() == 1)
			error_pack.status3 &= ~(1<<0);
		else
 			error_pack.status3 &= ~(1<<1);
     	}
	if(other_default_data.tms_status.tms_valid)
	{
		if(read_pisc_id() == 2)
			error_pack.status3 |= (1<<0);   
		else
			error_pack.status3 |= (1<<1);   
	}
	else
    	{
		if(read_pisc_id() == 2)
			error_pack.status3 &= ~(1<<0);
		else
 			error_pack.status3 &= ~(1<<1);
    	}			

	//����״̬����      
	for(lcu_cnt = 0; lcu_cnt < TBA_LCU_NUM; lcu_cnt++)
	{
	    memcpy((uint8*)error_pack.srck[lcu_cnt], (uint8*)&(lcu_default_data[lcu_cnt]), sizeof(lcu_default_data_t));
	}
			
	//ʮ���ӽ���һ���쳣��⣬д��flash
     if((my_default_data.status_hbit.master==1) && (checktimer(&check_time, ERROR_CHECK_TIME))&& error_record_flag == 0)
     {
            //���쳣����,дflash      ���һλerror_no���Ƚ�
	        if(memcmp((uint8*)&error_pack, (uint8*)&(saveparam.pre_error_pack), (sizeof(error_packet_t))) != 0)
	        {
	        	num++;
		       //error_pack.error_no = num%255;
	            	memcpy((uint8*)&(saveparam.pre_error_pack), (uint8*)&error_pack, sizeof(error_packet_t));

		       //��ȡʱ��ʱ��
		       rtc_dev_p->get_rtc(rtc_time);
		       memcpy((uint8*)buf, (uint8*)rtc_time, sizeof(rtc_time));
		       memcpy((uint8*)(buf+sizeof(rtc_time)), (uint8*)&error_pack, sizeof(error_packet_t));
		       print_int("%x", rtc_time[0]);
		       print_int("%x", rtc_time[1]);
		       print_int("%x", rtc_time[2]);
		       print_int("%x", rtc_time[3]);
		       print_int("%x", rtc_time[4]);

		       //дflash
		       if((saveparam.error_flash_addr + ONE_ERROR_RECORD) > Endaddr)
		       {
		       	//��ͷ��ʼд
		           	saveparam.error_flash_addr = 0x00;
		           	saveparam.re_write_flash = 1;
		       }

                	addr_temp1 = saveparam.error_flash_addr%PAGE_SIZE;
		       addr_tmp0 = (saveparam.error_flash_addr/PAGE_SIZE)*PAGE_SIZE;
		       if(PAGE_SIZE-addr_temp1 < ONE_ERROR_RECORD)
		       {
		           	read_flash(addr_tmp0, write_buf, addr_temp1);

		           	memcpy((uint8*)(write_buf+addr_temp1),buf, PAGE_SIZE-addr_temp1);
                   		write_flash(addr_tmp0, write_buf, PAGE_SIZE);
			 
		           	mdelay(150);
		           	//write to next page
		           	write_flash(addr_tmp0+PAGE_SIZE, (uint8*)&buf[(PAGE_SIZE-addr_temp1)], ONE_ERROR_RECORD-(PAGE_SIZE-addr_temp1));
                	}
		       else
		       {
		            	read_flash(addr_tmp0, write_buf, addr_temp1);
		           	memcpy((uint8*)(write_buf+addr_temp1),buf, ONE_ERROR_RECORD);
		            	write_flash(addr_tmp0, write_buf, addr_temp1+ONE_ERROR_RECORD);
		       }

		       //write_flash(saveparam.error_flash_addr, buf, ONE_ERROR_RECORD);
		       saveparam.error_flash_addr += ONE_ERROR_RECORD;
		       save_set_task();    //�������
		       print_line("save error data");
		       print_buf(buf, ONE_ERROR_RECORD);
	        }
     }

}

//***********************************************************************************************************************
//��������:�豸������
//����˵��:��
//ע������:
//����˵��:
//***********************************************************************************************************************
void error_proc(void)
{

   //uint8 index = 0;
  // uint8 dev_type_tmp= 0;
   //uint8 dev_no_tmp = 0;
   //uint8 read_id_tmp;

   error_pecu_proc();
   //error_display_proc();
	//����״̬

	/*
   	sh9_tms_dev.tms_send_packet.dw2.ACSU_fail_car1=0;
	sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car1=0;
	sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car2=0;
	sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car3=0;
	sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car4=0;
	sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car5=0;
	sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car6=0;
	sh9_tms_dev.tms_send_packet.dw2.ACSU_fail_car6=0;
	*/

#if 0
   

   for(index = 0; index < POLL_NUM_MAX; index++)
   {
        if(token_polling_table[index].poll_error >= 10)     //Ѳ�쳬ʱ
        {
                 dev_type_tmp = token_polling_table[index].dev_id&0x0f;
		    dev_no_tmp = token_polling_table[index].dev_id >> 4;

			#if 0
		    if(dev_type_tmp == 1)//pisc
		    {
		       if(dev_no_tmp == 1)
		       {
		           sh9_tms_dev.tms_send_packet.dw2.ACSU_fail_car1=1;
		       }
			 else if(dev_no_tmp == 2)
			 {
			    sh9_tms_dev.tms_send_packet.dw2.ACSU_fail_car6=1;
			 }
			 
		    }
		    else if(dev_type_tmp == 3)//lcu
		    {
		       if(dev_no_tmp == 1)
		       {
		           sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car1=1;
		       }
			 else if(dev_no_tmp == 2)
		       {
		           sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car2=1;
		       }
			 else if(dev_no_tmp == 3)
		       {
		           sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car3=1;
		       }
			else if(dev_no_tmp == 4)
		       {
		           sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car4=1;
		       }
			else if(dev_no_tmp == 5)
		       {
		          sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car5=1;
		       }
			else if(dev_no_tmp == 6)
		       {
			    sh9_tms_dev.tms_send_packet.dw2.PACU_fail_car6=1;
		       }
		    }
			#endif
        }
    }
#endif
   save_error_info();
   print_error_info();
}


