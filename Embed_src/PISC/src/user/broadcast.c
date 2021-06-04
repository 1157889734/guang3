
/******************************************************************************
*�ļ����ƣ�broadcast.c		       		         
*��          �ߣ�hk           							 
*��          �����㲥����		
*�������ڣ�2010-3-11
*�޸ļ�¼����								
******************************************************************************/

#include "include.h"



//�㲥���Ͷ���
#define BROADCAST_TYPE_OCC		0x00	//���߹㲥
#define BROADCAST_TYPE_ICC		       0x01	//�˿ͱ���
#define BROADCAST_TYPE_TALK		0x02	//˾���Խ�
#define BROADCAST_TYPE_MANUAL	0x03	//�˹��㲥
#define BROADCAST_TYPE_DOOR		0x04	//������ʾ��
#define BROADCAST_TYPE_DVA		0x05	//���ֱ�վ
#define BROADCAST_TYPE_MEDIA		0x06	//ý�岥��
#define BROADCAST_TYPE_SPECIAL	0x07	//�����㲥
#define BROADCAST_TYPE_TEST		0x08	//������


#define REQUEST_PA_TIMES 6

static uint8 dcp_req_times=0;
static uint8 occ_req_times=0;

//APU ���� ����
static uint8 close_door_playtimes=0;
static uint8 close_times_flag=0;
static uint32 close_door_times=0;

static uint8 apu_play_status=0;
static uint32 apu_play_checktimes=0;

static uint8 special_broadcast_times=0;//����㲥����
static uint8 door_chime_dva=0;
static uint8 station_broadcast_dva=0;
static uint8 special_broadcast_dva=0;

static uint8 broadcast_priority = 0;    //�㲥���ȼ�
static uint8 request_PA_times = 0;
static uint8 door_pa_status = 0;

static void delay_n(uint32 n)
{
	for(;n>0;n--);
}


/* 
	1:�����ϵ�����;  2.������������   3:��͵���خ  4:��خ�����
	5:ͬ�͵���ʯ     6.��ʯ��ͬ��     7:ͬ�͵�����  8:������ͬ��  
	9: ��������خ  10. ��خ������
*/
void routeid_handle (void)
{
	static uint8 pre_route_id = 0;
	// 1:������/������������
	if((sh9_tms_dev.start_station >17 && sh9_tms_dev.start_station < 26)
		&& sh9_tms_dev.end_station ==11)
		sh9_tms_dev.route_id = 1;

	// 2.������������/������ 
	else if((sh9_tms_dev.start_station<24 && sh9_tms_dev.start_station >10)
		&& ((sh9_tms_dev.end_station ==24)||(sh9_tms_dev.end_station ==25)))
		sh9_tms_dev.route_id = 2;

	// 3:��͵���خ  
	else if( sh9_tms_dev.start_station > 25 && sh9_tms_dev.end_station ==1)
		sh9_tms_dev.route_id = 3;

	// 4:��خ�����
	else if( (sh9_tms_dev.start_station <4 && sh9_tms_dev.start_station >0) 
		&& sh9_tms_dev.end_station ==30)
		sh9_tms_dev.route_id = 4;

	// 5:ͬ�͵���ʯ 
	else if( sh9_tms_dev.start_station <18 && sh9_tms_dev.end_station ==4)
		sh9_tms_dev.route_id = 5;

	// 6.��ʯ��ͬ�� 
	else if( (sh9_tms_dev.start_station <10 && sh9_tms_dev.start_station>3)
		&& sh9_tms_dev.end_station ==17)
		sh9_tms_dev.route_id = 6;

	// 7:ͬ�͵�����
	else if( (sh9_tms_dev.start_station <18 && sh9_tms_dev.start_station>11)
		&& sh9_tms_dev.end_station ==11)
		sh9_tms_dev.route_id = 7;

	// 8:������ͬ��
	else if( (sh9_tms_dev.start_station <17 && sh9_tms_dev.start_station>10)
		&& sh9_tms_dev.end_station ==17)
		sh9_tms_dev.route_id = 8;

	// 9: ������/������خ
	else if( (sh9_tms_dev.start_station <26 && sh9_tms_dev.start_station>17)
		&& sh9_tms_dev.end_station ==1)
		sh9_tms_dev.route_id = 9;

	// 10. ��خ��������/��
	else if( (sh9_tms_dev.start_station <11)&& (sh9_tms_dev.end_station ==24 
		||sh9_tms_dev.end_station ==25))
		sh9_tms_dev.route_id = 10;

	// 11. ��ӿ���վ/��ʯ
	else if( sh9_tms_dev.start_station > 25 && sh9_tms_dev.end_station ==4)
		sh9_tms_dev.route_id = 11;

    // 12.��ʯ/��ӿ���վ
	else if( (sh9_tms_dev.start_station <10 && sh9_tms_dev.start_station >3)
		&& sh9_tms_dev.end_station ==30)
		sh9_tms_dev.route_id = 12;

	// 13.ͬ�͵���خ�㳡
	else if((sh9_tms_dev.start_station <18 && sh9_tms_dev.start_station>11)
		&& sh9_tms_dev.end_station ==1)
		sh9_tms_dev.route_id = 13;

	// 14.��خ�㳡��ͬ��
	else if((sh9_tms_dev.end_station <18 && sh9_tms_dev.end_station>11) 
		&& sh9_tms_dev.start_station ==1)
		sh9_tms_dev.route_id = 14;	
	
	else
		sh9_tms_dev.route_id = 0xff;

	if(pre_route_id != sh9_tms_dev.route_id )
		print_int("%droute_chg:", sh9_tms_dev.route_id );
	pre_route_id = sh9_tms_dev.route_id ; 
}

void leave_broadcast()
{
	dva_dev->dva_clear();					 
       station_broadcast_dva = 1;//��վ�㲥��ʾ
	my_default_data.trigger_hbit.leave_broadcast=1; 
	my_default_data.trigger_hbit.arrived_broadcast = 0;						

	// 1. ����+ ...���в���
	dva_dev->play_list(502);
					
	// 2. �����г�����
	dva_dev->play_list(400+sh9_tms_dev.end_station); 

	// 3. ��һվ+ ����
	if(sh9_tms_dev.next_station != sh9_tms_dev.end_station) // ��һվ�����յ�վ
	{
		if(sh9_tms_dev.next_station == 11) //������·
		{
			#if 1							
			if(sh9_tms_dev.route_id ==3)// ��� to ��خ
				dva_dev->play_list(1261);

                      else if(sh9_tms_dev.route_id ==4)// ��خ to ���
                              dva_dev->play_list(261); 
						
			else if(sh9_tms_dev.route_id ==5)// ͬ�� to ��ʯ
				dva_dev->play_list(1262);
			
			else if(sh9_tms_dev.route_id ==6)// ��ʯ to ͬ��
				dva_dev->play_list(262);

			else if(sh9_tms_dev.route_id ==9 )// ���� to ��خ
				dva_dev->play_list(1263);
			
			else if(sh9_tms_dev.route_id ==10)// ��خ to ����
				dva_dev->play_list(263);
			
			else if(sh9_tms_dev.route_id ==11)// 11.��� to ��ʯ
				dva_dev->play_list(1266);
			
			else if(sh9_tms_dev.route_id ==12)// 12.��ʯ to ���
				dva_dev->play_list(266);      //���÷�خ to ���

			else {
                                if(sh9_tms_dev.start_station<sh9_tms_dev.end_station) // ����
                                    dva_dev->play_list(200+sh9_tms_dev.next_station); 
                                else
                                    dva_dev->play_list(1200+sh9_tms_dev.next_station);  // ����
                      }
			#endif
		}
		else{
                    if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                        dva_dev->play_list(200+sh9_tms_dev.next_station); 
                    else
                        dva_dev->play_list(1200+sh9_tms_dev.next_station); // ����
               }
	}
	else    // ��һվ���յ�վ
	{
		if(sh9_tms_dev.next_station == 11 ) //������·
		{
		       if(sh9_tms_dev.route_id ==7)  // ͬ�� to ����
				dva_dev->play_list(362);
			else if(sh9_tms_dev.route_id ==1)  // ������/�� to ����
				dva_dev->play_list(361);
			else
				dva_dev->play_list(300+sh9_tms_dev.next_station); 
		}
		else
			dva_dev->play_list(300+sh9_tms_dev.next_station); 
	}

	// 4. վ������Ϣ
	if(sh9_tms_dev.next_station == 9) //������
	{
	
		 if(sh9_tms_dev.route_id ==10)// ��خ to ����
		 {
			dva_dev->play_list(141);   
		 }
		 else if(sh9_tms_dev.route_id ==6) // ��ʯ to ͬ��
		 {
			dva_dev->play_list(141);   

		 }
		 else if(sh9_tms_dev.route_id ==14) // ��خ�㳡 to ͬ��
		 {
			dva_dev->play_list(141);   

		 }
		 else
		 {
              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
              else
                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
		 }
		

	}
	else if(sh9_tms_dev.next_station == 10) //�齭�³�
	{

		 if(sh9_tms_dev.route_id ==10)// ��خ to ����
		 {
			dva_dev->play_list(151);   

		 }
		 else if(sh9_tms_dev.route_id ==6) // ��ʯ to ͬ��
		 {
			dva_dev->play_list(151);   

		 }
		 else if(sh9_tms_dev.route_id ==14) // ��خ�㳡 to ͬ��
		 {
			dva_dev->play_list(151);   

		 }
		 else
		 {
              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
              else
                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
		 }

	}
	
	
	else if(sh9_tms_dev.next_station == 11) //������·
	{
		if(sh9_tms_dev.next_station == sh9_tms_dev.end_station)
		{
			
			 if(sh9_tms_dev.route_id ==7)  // ͬ�� �� ����
				dva_dev->play_list(1163);
			
			 else if(sh9_tms_dev.route_id == 1)  // ������/�� �� ����
				dva_dev->play_list(1162);
			
			 else{
                              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
                              else
                                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
                       }		 
		}
		else
		{
			if(sh9_tms_dev.route_id ==3) // ��� to ��خ
				dva_dev->play_list(1161);
                       if(sh9_tms_dev.route_id ==4) // ��خ to ���
                              dva_dev->play_list(161);
			else if(sh9_tms_dev.route_id ==5) // ͬ�� to ��ʯ
				dva_dev->play_list(1164);
			else if(sh9_tms_dev.route_id ==6) // ��ʯ to ͬ��
				dva_dev->play_list(164);            
			else if(sh9_tms_dev.route_id ==9)// ���� to ��خ
				dva_dev->play_list(1165);
			else if(sh9_tms_dev.route_id ==10)// ��خ to ����
				dva_dev->play_list(165);
			else if(sh9_tms_dev.route_id ==11)// 11.��� to ��ʯ
				dva_dev->play_list(1161);     //������� to ��خ
			else if(sh9_tms_dev.route_id ==12)// 12.��ʯ to ���
				dva_dev->play_list(161);      //���÷�خ to ���

			
   			else{
                              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
                              else
                                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
                       }
		}
	}
	else if(sh9_tms_dev.next_station == 12) //�ֺ���
	{
		if (sh9_tms_dev.route_id ==1)         // ������/�� to ����
			dva_dev->play_list(1171);		
               else if(sh9_tms_dev.route_id ==2)  // ���� to ������/��
			dva_dev->play_list(171);		
               else if(sh9_tms_dev.route_id ==5) // ͬ�� to ��ʯ
			dva_dev->play_list(1172);		
               else if(sh9_tms_dev.route_id ==6) // ��ʯ to ͬ��
			dva_dev->play_list(172);		
               else if(sh9_tms_dev.route_id ==7) // ͬ�� to ����
			dva_dev->play_list(1173);		
               else if(sh9_tms_dev.route_id ==8) // ���� to ͬ��
			dva_dev->play_list(173);		                
		else{
                      if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                          dva_dev->play_list(100+sh9_tms_dev.next_station); 
                      else
                          dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
               }	
	}
	else if(sh9_tms_dev.next_station == 16)   // ��Ϫ�Ϸ�ҽԺ
	{	
		if(sh9_tms_dev.route_id ==8)  // ���� �� ͬ��
			dva_dev->play_list(191);
		else{
                      if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                          dva_dev->play_list(100+sh9_tms_dev.next_station); 
                      else
                          dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
               }			
	}
	else if(sh9_tms_dev.next_station == 17) //ͬ��
	{
		if (sh9_tms_dev.route_id ==1)         // ������/�� to ����
			dva_dev->play_list(1181);		
               else if(sh9_tms_dev.route_id ==2)  // ���� to ������/��
			dva_dev->play_list(181);		
               else if(sh9_tms_dev.route_id ==6) // ��ʯ to ͬ��
			dva_dev->play_list(182);		
               else if(sh9_tms_dev.route_id ==8) // ���� to ͬ��
			dva_dev->play_list(183);	
		else{
                      if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                          dva_dev->play_list(100+sh9_tms_dev.next_station); 
                      else
                          dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
               }	
	}
	else{
              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // ����
                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
              else
                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // ����
        }

	// 5. �յ�վ��ʾ��
	if(sh9_tms_dev.next_station == sh9_tms_dev.end_station)
		dva_dev->play_list(601); 						
      				
	print_line("leave broadcast_proc");
}


//***********************************************************************************************************************
//��������:�㲥����
//����˵��:��
//ע������:
//����˵��:
//***********************************************************************************************************************
void broadcast_proc(void)
{
	uint8 channel;
	uint8 temp_priority[BROADCAST_NUM_MAX];//���ڱȽϵ���ʱ���ȼ���
	uint8 broadcast_flag=0;//��ǰDVA�㲥��־
	uint8 special_broadcast_flag = 0;  //����㲥��־
	uint8 dva_priority[3];
	//uint16 mp3_tmp = 0;
	uint8 tmp = 0;
	//uint8 station_id=0;
	uint8 i,j;

	routeid_handle();
	

	//-----------------------DVA�㲥����---------------------------------
	//��ֹ��;��վ Ӱ����ʾ��
	if(occ_req_times != 1 && dcp_req_times != 1)
	{

		//����㲥--���ȴ���
		special_broadcast_flag = tms_dev->get_special_broadcast();

		broadcast_flag = tms_dev->get_station_broadcast();

       dva_priority[0] = BROADCAST_TYPE_SPECIAL;
		dva_priority[1] = BROADCAST_TYPE_DVA;
		dva_priority[2] = BROADCAST_TYPE_DOOR;
		for(i = 0; i < 2; i++)
		{
		    for(j = i+1; j < 3; j++)
		    	{
		    	    if(saveparam.broadcast_priority[dva_priority[i]]< saveparam.broadcast_priority[dva_priority[j]])
		    	    	{
		    	    	    tmp = dva_priority[i];
						    dva_priority[i] = dva_priority[j];
						    dva_priority[j] = tmp;
		    	    	}
		    	}
		}
             tmp = 0;
		while(tmp < 3)
		{
		    if(!my_default_data.status_hbit.master)  //���������㲥
			break;
			
		    //�����㲥
		    if((dva_priority[tmp] == BROADCAST_TYPE_SPECIAL) &&(special_broadcast_flag) 
				&& (broadcast_priority <= saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL]))
		    	{
		    	      if(tms_dev->special_broadcast_code != my_default_data.urgent_broadcast_no)
						{
					        my_default_data.urgent_broadcast_no = tms_dev->special_broadcast_code;//�����㲥����
					        special_broadcast_times = 0;
							  dva_dev->stop();
							  print_line("sp");
						}

						if(tms_dev->special_broadcast_code > (sizeof(saveparam.special_broadcast_times)-1))
						{
						   special_broadcast_times=saveparam.special_broadcast_times[1];
						}
						else
						{
				    	          special_broadcast_times=saveparam.special_broadcast_times[tms_dev->special_broadcast_code];
						}
					      special_broadcast_dva = 1;
						//���Ͳ�������				
						dva_dev->dva_clear();						
						//dva_dev->play_special(tms_dev->special_broadcast_code+EMERGENCY_PA_BASE_ID);		
						dva_dev->play_special(tms_dev->special_broadcast_code);	
						print_line("special br");
						special_broadcast_times--;
						break;
		    	}
				
			//������ʾ��
		    else if((dva_priority[tmp] == BROADCAST_TYPE_DOOR) &&(broadcast_flag == 3) 
				&& (broadcast_priority <= saveparam.broadcast_priority[BROADCAST_TYPE_DOOR]))
		    	{
		    	    //�ж��Ƿ񲥷Ź�����ʾ��
				if(saveparam.door_broadcast_flag)
				{
					close_door_playtimes=saveparam.close_door_playtimes;
					print_int("%x close:", close_door_playtimes);
				      dva_dev->dva_clear();  
					dva_dev->stop();
					 
					//---������ʾ�����Ŵ���
					if(saveparam.door_broadcast_flag)
					{
						//ptu_print_debug("d2");
						//apu_dev->set_door_out(0);
						close_door_times= get_clock();	
						apu_play_checktimes=get_clock();
						apu_play_status=1;
						apu_dev->set_door_out(1); //������ʾ����apu��
						
					}
					//���û�в��� ���ܲ���
					//if(!door_chime_dva)
					{
						//���Ͳ�������
						#if NAME_RULE_NEW
						//dva_dev->play_list(saveparam.dva_door_list[0]); //����������APU����
						#else
						dva_dev->play_list(saveparam.dva_door_list);
						#endif
						print_line("door br");
						door_chime_dva=1;
						my_default_data.trigger_hbit.arrived_broadcast =0;
						door_pa_status = 0 ;
					}
					break;
				}
				
		    	}
			else if((dva_priority[tmp] == BROADCAST_TYPE_DOOR) &&(broadcast_flag == 4) 
				&& (broadcast_priority <= saveparam.broadcast_priority[BROADCAST_TYPE_DOOR])
				&& (!door_pa_status))
		    	{
		    	    //�ж��Ƿ񲥷Ź�����ʾ��
				if(saveparam.door_broadcast_flag)
				{
				      dva_dev->dva_clear();
					//���û�в��� ���ܲ���
					//if(!door_chime_dva)
					{
						//���Ͳ�������
						#if NAME_RULE_NEW
						//dva_dev->play_list(saveparam.dva_door_list[0]); //����������APU����
						#else
						dva_dev->play_list(saveparam.dva_door_list);
						#endif
						print_line("door open br");
						door_chime_dva=1;
						//broadcast_flag =0;
						 //tms_dev->set_broadcast(0);
						//my_default_data.trigger_hbit.arrived_broadcast =0;
						door_pa_status = 1;
					}
					break;
				}
				
		    	}

			//��վ�㲥
		    else if((dva_priority[tmp] == BROADCAST_TYPE_DVA) &&((broadcast_flag== 1) || (broadcast_flag== 2)) 
				&& (broadcast_priority <= saveparam.broadcast_priority[BROADCAST_TYPE_DVA]))
		    	{		    	    
		    	    	if( broadcast_flag == 1) //��վ�㲥����
		    	    	{
					leave_broadcast();
				}
				else if( broadcast_flag == 2 )//��վ�㲥
				{
					//�ͱ�־
					my_default_data.trigger_hbit.leave_broadcast=0;
					my_default_data.trigger_hbit.arrived_broadcast=1;
					print_line("arrive broadcast");
				}
				break;
		    	}
			tmp++;
		}
	
		my_default_data.trigger_hbit.urgent_broadcast = (special_broadcast_times != 0);//�ͽ����㲥��־
	}


	/*****apu ������ʾ�����Ŵ���*******/
	#if 1
	#if 1
	if( checktimer(&close_door_times,15000))//15s û�õ��ظ�ǿ���ж�APU���Ų���
	{
		close_door_playtimes=0;
		door_chime_dva = 0;
		close_times_flag = 0;
		close_door_times = get_clock();
		ptu_print_debug("10S");		
	}
	#endif
	if( checktimer(&apu_play_checktimes,1500))// ��ʱ1s ��apu����״̬ 
	{
		if (!apu_dev->get_door_in())
		{
			apu_play_status = 0;
		}
		else
			apu_play_status=1;
	}
	
	if (!apu_play_status) //apu ������ʾ���������
	{
		if(close_door_playtimes==0) //���Ŵ���Ϊ0 ��ֹͣ����
		{
			apu_dev->set_door_out(0);
			door_chime_dva=0;
			//ptu_print_debug("cd");
		}
		
		if(close_door_playtimes>0&&close_door_playtimes<0xff)
		{
			//print_line("d1");
			if(close_times_flag==0)
			{
				apu_dev->set_door_out(0);//���� Ϊ���沥���ṩ�½��ش�������
				delay_n(5000);
				close_door_playtimes--;
				close_times_flag=1;
				close_door_times= get_clock();					
			}
			
		}
		else
			close_door_playtimes=0;	
		
		
		if(close_door_playtimes&&occ_req_times == 0)//������ʾ�����Ŵ���
		{
			if(close_door_playtimes>0)
			{
				door_chime_dva = 1;
				special_broadcast_dva = 0;
				station_broadcast_dva = 0;
				broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_DOOR];
				//---������ʾ�����Ŵ���
				if(saveparam.door_broadcast_flag)
				{
					//ptu_print_debug("d2");
					//apu_dev->set_door_out(0);
					if(close_times_flag)
					{
						apu_dev->set_door_out(1); //������ʾ����apu��
						apu_play_checktimes = get_clock();
						apu_play_status=1;
						print_line("close+");
						//print_line("d2");
					}
				}
				
			}
		}
		
	}
	else  //apu ������ʾ�����ڲ���
	{
		close_times_flag=0;
		//ptu_print_debug("nd");
	}
	#endif

	/*****dva ���Ŵ���*******/
	if(dva_dev->get_playing())//���ڹ㲥
	{
	      
		//TMSҪ��ֹͣ��ǰ�㲥
		//DCP��ȡ���㲥��ť 
		if(sh9_tms_dev.emergency_status )//|| tms_dev->dva_stop)
		{
			//����ֹͣ����
			dva_dev->stop();
			sh9_tms_dev.emergency_status = 0;  //��ֹͣ����㲥��־
			//�������㲥�Ĵ���
			special_broadcast_times=0;
			print_line("stop emg");
		}
	}
	else			//�㲥���
	{
	       if(sh9_tms_dev.emergency_status )//DCPֹͣ�����㲥
		{
			//�������㲥�Ĵ���
			special_broadcast_times=0;
			sh9_tms_dev.emergency_status = 0;  //��ֹͣ����㲥��־
		}
		   
		//�ж��Ƿ�������㲥����
		if(special_broadcast_times && occ_req_times == 0)
		{
			//���Ͳ�������
			if(special_broadcast_times>1)
			{
			      //for test
				//dva_dev->play_special(812);
				if(tms_dev->special_broadcast_code < 1)
					tms_dev->special_broadcast_code = 1;//��ֹ����ļ�����ָ���
				
				dva_dev->play_special(tms_dev->special_broadcast_code);
				
				//dva_dev->play_special(tms_dev->special_broadcast_code+9000);
				broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL];
				//dva_dev->play_special(901);
				special_broadcast_dva = 1;
				door_chime_dva = 0;
				station_broadcast_dva = 0;
				print_line("special br+");
			}
			//�������� ���ж��Ƿ����޴���
			if(special_broadcast_times != 0xFF)
				special_broadcast_times--;
		}
		else
		{		
			//����Ź㲥
			//door_chime_dva=0;
			special_broadcast_dva= 0;
			station_broadcast_dva=0;

			broadcast_priority = 0;
			request_PA_times = 0;
		}
	}

	
	media_send_packet.status.emergency = 0;
	//------------------------�㲥ͨ�����ȼ�Ѳ����л�-------------------------
	//��������� ֱ�ӵ�û���κι㲥
	if( !my_default_data.status_hbit.master )
		goto no_out;
	
	//�͹������ݵĹ㲥���ȼ�
	my_default_data.broadcast_priority.pecu = saveparam.broadcast_priority[BROADCAST_TYPE_ICC];
	my_default_data.broadcast_priority.talk = saveparam.broadcast_priority[BROADCAST_TYPE_TALK];
	my_default_data.broadcast_priority.manual = saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL];
	my_default_data.broadcast_priority.occ = saveparam.broadcast_priority[BROADCAST_TYPE_OCC];
	my_default_data.broadcast_priority.media = saveparam.broadcast_priority[BROADCAST_TYPE_MEDIA];
	my_default_data.broadcast_priority.dva = saveparam.broadcast_priority[BROADCAST_TYPE_DVA];
	my_default_data.broadcast_priority.door = saveparam.broadcast_priority[BROADCAST_TYPE_DOOR];
	my_default_data.broadcast_priority.special = saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL];
	//�Ѳ�����������ʱ���ȼ���
	memcpy(temp_priority,(uint8 *)&saveparam.broadcast_priority, BROADCAST_NUM_MAX);
	//�������ȼ��������й㲥ͨ��

	for(channel=0; channel<BROADCAST_NUM_MAX; channel++)
	{
		uint8 broadcast_index;//ͨ��ѭ������
		uint8 highest_priority = 0;//��һ��������ȼ�
		uint8 highest_index = 0xFF;//������ȼ�������

		
		//������һ�������ȼ�
		for(broadcast_index=0;broadcast_index<BROADCAST_NUM_MAX;broadcast_index++)
		{
			//���ϴ�ѭ���������ͨ���ų�
			if(temp_priority[broadcast_index])
			{
				//�Ƚϲ������ֵ
				if(temp_priority[broadcast_index]>=highest_priority)
				{
					highest_priority=temp_priority[broadcast_index];
					highest_index=broadcast_index;
				}
			}
		}
		
		//����ҵ���һ�������ȼ� ���ദ���ҵ�ͨ���Ĺ㲥
		if(highest_index<BROADCAST_NUM_MAX)
		{			
			//�����ʱ���ȼ�������Ӧ����
			temp_priority[highest_index]=0;
					
			//OCC		�ж�OCC�����,  OCC���ȼ����ڵ��ڵ�ǰ���ȼ�
			if((highest_index==BROADCAST_TYPE_OCC) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{     
				if(apu_dev->get_occ_in())
				{
					dcp_req_times=0;					
					
					//�ж��Ƿ�������
					//paռ�ò���Ϊdcpռ�ã�����dcp���ڹ㲥״̬
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_OCC;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//��Դָʾ��
						led_blink(BROADCAST_OCC_LED_NO,250,250);
						led_onoff(BROADCAST_DVA_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						request_PA_times++;
					}
					//�ж� ֱ���л�
					else
					{
					       occ_req_times = 2;

					#if 0     //ȡ����ʾ��
						//��һ�����벥����ʾ��
						if(occ_req_times==0)
						{
							//OCC�㲥��ʾ��
							uint16 occ_pre_sound[]={800,0};
							
							occ_req_times=1;
							//������ʾ��
							#if NAME_RULE_NEW
							dva_dev->play_list(occ_pre_sound[0]);
							#else
							dva_dev->play_list(occ_pre_sound);
							#endif
							
							print_line("OCC broadcast");
							//�͹�������
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
							my_default_data.broadcast_signal.broadcast_req=0;
							my_default_data.broadcast_signal.broadcasting=1;
							//�л�ͨ��
							apu_dev->set_channel(APU_CHANNEL_DVA);
							//ռ��PA��
							if(!apu_dev->get_pa_out())
							       apu_dev->set_pa_out(1);
							//��Դָʾ��
							led_onoff(BROADCAST_OCC_LED_NO,1);
							led_onoff(BROADCAST_DVA_LED_NO,0);
							led_onoff(BROADCAST_MEDIA_LED_NO,1);
						}
						//���ǵ�һ������ �ȴ��������
						else if(occ_req_times==1)
						{
							//������� ��������� �ͷ�PA
							if(!dva_dev->get_playing())
							{
								occ_req_times=2;
								
							}
							
						}
					#endif
						if(occ_req_times==2)
						{				
							//�͹�������
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_OCC;
							my_default_data.broadcast_signal.broadcast_req=0;
							my_default_data.broadcast_signal.broadcasting=1;
							//ռ��PA��
							if(!apu_dev->get_pa_out())
							       apu_dev->set_pa_out(1);
							//�л�ͨ��
							apu_dev->set_channel(APU_CHANNEL_OCC);
							//��Դָʾ��
							led_onoff(BROADCAST_OCC_LED_NO,0);
							led_onoff(BROADCAST_DVA_LED_NO,1);
							led_onoff(BROADCAST_MEDIA_LED_NO,1);
							
						}
					}
					//��ǰΪOCC�㲥
					broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_OCC];
					// should delete
				        //broadcast_priority = 0;
					//debug_print_line("occ return");
					
					return;
				}
			
				else if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_OCC])
				{
				    //��ǰ���ȼ�Ϊocc�㲥�����Ѳ���occ�㲥�����㵱ǰ�㲥���ȼ�
				    broadcast_priority = 0;
				   occ_req_times = 0;
				}

                            // should delete
				//broadcast_priority = 0;

			}

			/***************��3������Ƶ����  CC  PC************************/

			// CC �Խ�  �жϱ�վ
			else if((highest_index==BROADCAST_TYPE_TALK) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{
				if(dcp_get_talk_state()) //�жԽ�
				{
					//print_line("cc");
					dva_dev->stop();
					apu_dev->set_pa_out(0);
					broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_TALK];			
					return;
				}

				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_TALK])
				{	
				    broadcast_priority = 0;
				}
			}

			// PC �Խ�  �жϱ�վ
			else if((highest_index==BROADCAST_TYPE_ICC) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{
				if(dcp_get_emtalking_state()) //�жԽ�
				{
					//print_line("pc");
					dva_dev->stop();
					apu_dev->set_pa_out(0);
					broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_ICC];			
					return;
				}

				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_ICC])
				{	
				    broadcast_priority = 0;
				}
			}
		
			//DCP		�ж�DCP����͹㲥��־,�˹��㲥���ȼ����ڵ��ڵ�ǰ���ȼ�
			else if((highest_index==BROADCAST_TYPE_MANUAL) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{
			     //  uart0_write_buf((uint8*)&dcp_default_data[0], sizeof(dcp_default_data_t));
			      // uart0_write_char(dcp_default_data[0].broadcast_hbit.broadcast_req);
				//   uart0_write_char(dcp_get_broadcast_state());
				//�˹��㲥���� ������ʾ��
				if(dcp_get_broadcast_req())
				{
					occ_req_times=0;
					door_chime_dva = 0;
					special_broadcast_dva= 0;
					station_broadcast_dva=0;
					special_broadcast_times = 0;

					//dcp_req_times = 0;

					//print_line("n");
                                 #if 1
					//print_line("n");
					//��һ�����벥����ʾ��
					if(dcp_req_times==0)
					{
						//�˹��㲥��ʾ��
						dva_dev->dva_clear();
						dcp_req_times=2;
						//dcp_req_times=1;
						//������ʾ��
						//dva_dev->play_list(233);
						//dva_dev->play_list(501);
						
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//�л�ͨ��
						apu_dev->set_channel(APU_CHANNEL_DVA);
						//ռ��PA��
						if(!apu_dev->get_pa_out())
						     apu_dev->set_pa_out(1);
						//��Դָʾ��
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_DVA_LED_NO,0);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
					}
					//���ǵ�һ������ �ȴ��������
					else if(dcp_req_times==1)
					{
						//������� ��������� �ͷ�PA
						if(!dva_dev->get_playing())
						{
							dcp_req_times=2;
						}
					}
					#endif
					if(dcp_req_times==2)
					{
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_NONE;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=0;					
						//�л�ͨ��
						apu_dev->set_channel(APU_CHANNEL_NONE);
						//�ͷ�PA��

						//if(apu_dev->get_pa_out())
              					apu_dev->set_pa_out(0);
						//��Դָʾ��
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_DVA_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
					}

					 //dcp�˹��㲥
                                 broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL];

					//debug_print_line("P1");
					//�˳�ѭ��
					return;						
				}
				//�����˹��㲥 ֱ���л�ͨ��
				else if(dcp_get_broadcast_state())
				{
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					special_broadcast_dva= 0;
					station_broadcast_dva=0;
					special_broadcast_times = 0;
					
					//�͹�������
					my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_NONE;
					my_default_data.broadcast_signal.broadcast_req=0;
					my_default_data.broadcast_signal.broadcasting=0;				
					//�л�ͨ��
					apu_dev->set_channel(APU_CHANNEL_NONE);
					//�ͷ�PA��
                                if(apu_dev->get_pa_out())
					    apu_dev->set_pa_out(0);
					//��Դָʾ��
					led_onoff(BROADCAST_OCC_LED_NO,1);
					led_onoff(BROADCAST_DVA_LED_NO,1);
					led_onoff(BROADCAST_MEDIA_LED_NO,1);

					//dcp�˹��㲥
                                 broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL];

					//debug_print_line("P2");
					//�˳�ѭ��
					return;
				}
				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL])
				{	
				    broadcast_priority = 0;
				}
			}
			//������ʾ��	�ж�TMS���͵Ĺ�����ʾ��,������ʾ�����ȼ����ڵ��ڵ�ǰ���ȼ�
			else if((highest_index==BROADCAST_TYPE_DOOR) && ( broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{
				if(door_chime_dva)
				{
					//ptu_print_debug("d1");
					dcp_req_times=0;
					occ_req_times=0;
					special_broadcast_dva= 0;
					station_broadcast_dva=0;
					special_broadcast_times = 0;
					
					//�ж��Ƿ�������
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//�͹�������
						if(saveparam.door_broadcast_flag)
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DOOR;
						else
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						
						//��Դָʾ��
						if(saveparam.door_broadcast_flag)
						{
							led_blink(BROADCAST_DVA_LED_NO,250,250);
							led_onoff(BROADCAST_OCC_LED_NO,1);
							led_onoff(BROADCAST_MEDIA_LED_NO,1);
						}
						else
						{
							led_blink(BROADCAST_MEDIA_LED_NO,250,250);
							led_onoff(BROADCAST_OCC_LED_NO,1);
							led_onoff(BROADCAST_DVA_LED_NO,1);							
						}
					}
					//�ж� ֱ���л�
					else
					{
						//�͹�������
						if(saveparam.door_broadcast_flag)
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DOOR;
						else
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;					
						//ռ��PA��
						if(!apu_dev->get_pa_out())
							apu_dev->set_pa_out(1);
						//---������ʾ�����Ŵ���
						//�л�ͨ��
						apu_dev->set_channel(APU_CHANNEL_NONE);
						#if 0
						if(saveparam.door_broadcast_flag)
						{
							ptu_print_debug("d2");
							apu_dev->set_door_out(0);
							apu_dev->set_door_out(1); //������ʾ����apu��
							
						}
						#endif
						//��Դָʾ��
						#if 0
						if(saveparam.door_broadcast_flag)
						{
							led_onoff(BROADCAST_DVA_LED_NO,0);
							led_onoff(BROADCAST_OCC_LED_NO,1);
							led_onoff(BROADCAST_MEDIA_LED_NO,1);
						}
						else
						{
							led_onoff(BROADCAST_MEDIA_LED_NO,0);
							led_onoff(BROADCAST_OCC_LED_NO,1);
							led_onoff(BROADCAST_DVA_LED_NO,1);							
						}
						#endif

						broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_DOOR];
					}

					//print_line("DOOR");
					return;					
				}
				//��ǰΪ������ʾ�������Ѳ��꣬�㲥���ȼ�����
				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_DOOR])
					broadcast_priority = 0;
			}
			//DVA		�ж�DVA�����
			else if((highest_index==BROADCAST_TYPE_DVA) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{				
				if(station_broadcast_dva)
				{
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					special_broadcast_dva = 0;
					special_broadcast_times = 0;
					
					//�ж��Ƿ�������
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//��Դָʾ��
						led_blink(BROADCAST_DVA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						debug_print_line("3");
					}
					//�ж� ֱ���л�
					else
					{
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//ռ��PA��
						if(!apu_dev->get_pa_out())
						    apu_dev->set_pa_out(1);
						//�л�ͨ��
						//�ж��Ƿ�ͬʱ���ű�������
						if(saveparam.media_sametime)
						{
							apu_dev->set_channel(APU_CHANNEL_DVA_MEDIA);//�л�ͨ��
							//debug_print_line("AP1");
						}
						else
						{
							apu_dev->set_channel(APU_CHANNEL_DVA);//�л�ͨ��
							//debug_print_line("AP2");
						}
						//��Դָʾ��
						led_onoff(BROADCAST_DVA_LED_NO,0);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);

						broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_DVA];
					}

					//print_line("4");
					return;
				}
				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_DVA])
				{
				     broadcast_priority = 0;
				}

				//debug_print_line("dva play broadcast.c");
			}

			//�����㲥
			else if((highest_index==BROADCAST_TYPE_SPECIAL) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{				
				if(special_broadcast_times>=1)
				{
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					station_broadcast_dva = 0;
					
					//�ж��Ƿ�������
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_SPECIAL;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//��Դָʾ��
						led_blink(BROADCAST_DVA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						debug_print_line("3");
					}
					//�ж� ֱ���л�
					else
					{
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_SPECIAL;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//ռ��PA��
						if(!apu_dev->get_pa_out())
						    apu_dev->set_pa_out(1);
						//�л�ͨ��
						//�ж��Ƿ�ͬʱ���ű�������
						if(saveparam.media_sametime)
						{
							apu_dev->set_channel(APU_CHANNEL_DVA_MEDIA);//�л�ͨ��
							//debug_print_line("AP1");
						}
						else
						{
							apu_dev->set_channel(APU_CHANNEL_DVA);//�л�ͨ��
							//debug_print_line("AP2");
						}
						//��Դָʾ��
						led_onoff(BROADCAST_DVA_LED_NO,0);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);

						broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL];

					
						media_send_packet.status.emergency = 1;
					}

					//print_line("4");
					return;
				}
				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL])
				{
				     broadcast_priority = 0;
				}

				//debug_print_line("dva play broadcast.c");
			}

			else if((highest_index==BROADCAST_TYPE_TEST) 
				&& (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{				
				if(apu_dev->get_test_in()) //�в�������
				{
					ptu_print_debug("test");
					if(!dva_dev->get_playing())
						dva_dev->play_list(1000);
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					special_broadcast_dva = 0;
					special_broadcast_times = 0;
					
					//�ж��Ƿ�������
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//��Դָʾ��
						led_blink(BROADCAST_DVA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						debug_print_line("3");
					}
					//�ж� ֱ���л�
					else
					{
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//ռ��PA��
						if(!apu_dev->get_pa_out())
						    apu_dev->set_pa_out(1);
						//�л�ͨ��
						//�ж��Ƿ�ͬʱ���ű�������
						if(saveparam.media_sametime)
						{
							apu_dev->set_channel(APU_CHANNEL_DVA_MEDIA);//�л�ͨ��
							//debug_print_line("AP1");
						}
						else
						{
							apu_dev->set_channel(APU_CHANNEL_DVA);//�л�ͨ��
							//debug_print_line("AP2");
						}
						//��Դָʾ��
						led_onoff(BROADCAST_DVA_LED_NO,0);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);

						broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_TEST];
					}

					//print_line("4");
					return;
				}
				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_TEST])
				{
					dva_dev->dva_clear();
					dva_dev->stop();
				     broadcast_priority = 0;
				}

				//debug_print_line("dva play broadcast.c");
			}
#if 1
			//MEDIA		Ĭ����ý��������ҵ�ǰû������
			else if((highest_index==BROADCAST_TYPE_MEDIA)&&(LINK_CTRL_REG&(1<<LINK_CTRL_BIT))
				&& !other_default_data.test_status.reconnection)
			//if(0)
			{
				if(!my_default_data.status_hbit.active 
					&& !other_default_data.status_hbit.active
					&& !dcp_default_data[0].status_hbit.active
					&& !dcp_default_data[1].status_hbit.active) //����ûԿ�ײ���ý��
				{
					led_onoff(BROADCAST_DVA_LED_NO,1);
					led_onoff(BROADCAST_OCC_LED_NO,1);
					led_onoff(BROADCAST_MEDIA_LED_NO,1);
					apu_dev->set_channel(APU_CHANNEL_NONE);
					return;
				}
                                 	dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva= 0;
					
					//�ж��Ƿ�������
					if((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state())
					{
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//��Դָʾ��
						led_blink(BROADCAST_MEDIA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_DVA_LED_NO,1);
					}
					//�ж� ֱ���л�
					else
					{
						//�͹�������
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;					
						//ռ��PA��
						if(!apu_dev->get_pa_out())
					          apu_dev->set_pa_out(1);
						//�л�ͨ��
						apu_dev->set_channel(APU_CHANNEL_MEDIA);
						//��Դָʾ��
						led_onoff(BROADCAST_MEDIA_LED_NO,0);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_DVA_LED_NO,1);

						broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_MEDIA];
					}

					//debug_print_line("mdeia return");
					return;
			}
	#endif		
		}
		
	}


//û���κι㲥
no_out:
	//door_pa_status = 0;  //for test open door PA
	//tms_dev->set_broadcast(0);//for test open door PA
	dcp_req_times=0;
	occ_req_times=0;
	door_chime_dva = 0;
	special_broadcast_dva= 0;
	station_broadcast_dva=0;
	broadcast_priority = 0;
	//û���κ�ͨ���õ�PA����Ȩ �͹������ݹ㲥����
	my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_NONE;
	apu_dev->set_channel(APU_CHANNEL_NONE);//�л�ͨ��
	
         //if(apu_dev->get_pa_out())
	    apu_dev->set_pa_out(0);//�ͷ�PA��
	my_default_data.broadcast_signal.broadcast_req=0;
	my_default_data.broadcast_signal.broadcasting=0;
	//��Դָʾ��
	led_onoff(BROADCAST_OCC_LED_NO,1);
	led_onoff(BROADCAST_DVA_LED_NO,1);
	led_onoff(BROADCAST_MEDIA_LED_NO,1);	

}


