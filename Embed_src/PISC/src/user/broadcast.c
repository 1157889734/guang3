
/******************************************************************************
*文件名称：broadcast.c		       		         
*作          者：hk           							 
*描          述：广播处理		
*创建日期：2010-3-11
*修改记录：无								
******************************************************************************/

#include "include.h"



//广播类型定义
#define BROADCAST_TYPE_OCC		0x00	//无线广播
#define BROADCAST_TYPE_ICC		       0x01	//乘客报警
#define BROADCAST_TYPE_TALK		0x02	//司机对讲
#define BROADCAST_TYPE_MANUAL	0x03	//人工广播
#define BROADCAST_TYPE_DOOR		0x04	//关门提示音
#define BROADCAST_TYPE_DVA		0x05	//数字报站
#define BROADCAST_TYPE_MEDIA		0x06	//媒体播放
#define BROADCAST_TYPE_SPECIAL	0x07	//紧急广播
#define BROADCAST_TYPE_TEST		0x08	//测试音


#define REQUEST_PA_TIMES 6

static uint8 dcp_req_times=0;
static uint8 occ_req_times=0;

//APU 关门 处理
static uint8 close_door_playtimes=0;
static uint8 close_times_flag=0;
static uint32 close_door_times=0;

static uint8 apu_play_status=0;
static uint32 apu_play_checktimes=0;

static uint8 special_broadcast_times=0;//特殊广播次数
static uint8 door_chime_dva=0;
static uint8 station_broadcast_dva=0;
static uint8 special_broadcast_dva=0;

static uint8 broadcast_priority = 0;    //广播优先级
static uint8 request_PA_times = 0;
static uint8 door_pa_status = 0;

static void delay_n(uint32 n)
{
	for(;n>0;n--);
}


/* 
	1:机场南到体西;  2.体西到机场南   3:天客到番禺  4:番禺到天客
	5:同和到大石     6.大石到同和     7:同和到体西  8:体西到同和  
	9: 机场到番禺  10. 番禺到机场
*/
void routeid_handle (void)
{
	static uint8 pre_route_id = 0;
	// 1:机场南/机场北到体西
	if((sh9_tms_dev.start_station >17 && sh9_tms_dev.start_station < 26)
		&& sh9_tms_dev.end_station ==11)
		sh9_tms_dev.route_id = 1;

	// 2.体西到机场南/机场北 
	else if((sh9_tms_dev.start_station<24 && sh9_tms_dev.start_station >10)
		&& ((sh9_tms_dev.end_station ==24)||(sh9_tms_dev.end_station ==25)))
		sh9_tms_dev.route_id = 2;

	// 3:天客到番禺  
	else if( sh9_tms_dev.start_station > 25 && sh9_tms_dev.end_station ==1)
		sh9_tms_dev.route_id = 3;

	// 4:番禺到天客
	else if( (sh9_tms_dev.start_station <4 && sh9_tms_dev.start_station >0) 
		&& sh9_tms_dev.end_station ==30)
		sh9_tms_dev.route_id = 4;

	// 5:同和到大石 
	else if( sh9_tms_dev.start_station <18 && sh9_tms_dev.end_station ==4)
		sh9_tms_dev.route_id = 5;

	// 6.大石到同和 
	else if( (sh9_tms_dev.start_station <10 && sh9_tms_dev.start_station>3)
		&& sh9_tms_dev.end_station ==17)
		sh9_tms_dev.route_id = 6;

	// 7:同和到体西
	else if( (sh9_tms_dev.start_station <18 && sh9_tms_dev.start_station>11)
		&& sh9_tms_dev.end_station ==11)
		sh9_tms_dev.route_id = 7;

	// 8:体西到同和
	else if( (sh9_tms_dev.start_station <17 && sh9_tms_dev.start_station>10)
		&& sh9_tms_dev.end_station ==17)
		sh9_tms_dev.route_id = 8;

	// 9: 机场南/北到番禺
	else if( (sh9_tms_dev.start_station <26 && sh9_tms_dev.start_station>17)
		&& sh9_tms_dev.end_station ==1)
		sh9_tms_dev.route_id = 9;

	// 10. 番禺到机场南/北
	else if( (sh9_tms_dev.start_station <11)&& (sh9_tms_dev.end_station ==24 
		||sh9_tms_dev.end_station ==25))
		sh9_tms_dev.route_id = 10;

	// 11. 天河客运站/大石
	else if( sh9_tms_dev.start_station > 25 && sh9_tms_dev.end_station ==4)
		sh9_tms_dev.route_id = 11;

    // 12.大石/天河客运站
	else if( (sh9_tms_dev.start_station <10 && sh9_tms_dev.start_station >3)
		&& sh9_tms_dev.end_station ==30)
		sh9_tms_dev.route_id = 12;

	// 13.同和到番禺广场
	else if((sh9_tms_dev.start_station <18 && sh9_tms_dev.start_station>11)
		&& sh9_tms_dev.end_station ==1)
		sh9_tms_dev.route_id = 13;

	// 14.番禺广场到同和
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
       station_broadcast_dva = 1;//离站广播标示
	my_default_data.trigger_hbit.leave_broadcast=1; 
	my_default_data.trigger_hbit.arrived_broadcast = 0;						

	// 1. 叮咚+ ...往中部走
	dva_dev->play_list(502);
					
	// 2. 本次列车开往
	dva_dev->play_list(400+sh9_tms_dev.end_station); 

	// 3. 下一站+ 换乘
	if(sh9_tms_dev.next_station != sh9_tms_dev.end_station) // 下一站不是终点站
	{
		if(sh9_tms_dev.next_station == 11) //体育西路
		{
			#if 1							
			if(sh9_tms_dev.route_id ==3)// 天客 to 番禺
				dva_dev->play_list(1261);

                      else if(sh9_tms_dev.route_id ==4)// 番禺 to 天客
                              dva_dev->play_list(261); 
						
			else if(sh9_tms_dev.route_id ==5)// 同和 to 大石
				dva_dev->play_list(1262);
			
			else if(sh9_tms_dev.route_id ==6)// 大石 to 同和
				dva_dev->play_list(262);

			else if(sh9_tms_dev.route_id ==9 )// 机场 to 番禺
				dva_dev->play_list(1263);
			
			else if(sh9_tms_dev.route_id ==10)// 番禺 to 机场
				dva_dev->play_list(263);
			
			else if(sh9_tms_dev.route_id ==11)// 11.天客 to 大石
				dva_dev->play_list(1266);
			
			else if(sh9_tms_dev.route_id ==12)// 12.大石 to 天客
				dva_dev->play_list(266);      //沿用番禺 to 天客

			else {
                                if(sh9_tms_dev.start_station<sh9_tms_dev.end_station) // 上行
                                    dva_dev->play_list(200+sh9_tms_dev.next_station); 
                                else
                                    dva_dev->play_list(1200+sh9_tms_dev.next_station);  // 下行
                      }
			#endif
		}
		else{
                    if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                        dva_dev->play_list(200+sh9_tms_dev.next_station); 
                    else
                        dva_dev->play_list(1200+sh9_tms_dev.next_station); // 下行
               }
	}
	else    // 下一站是终点站
	{
		if(sh9_tms_dev.next_station == 11 ) //体育西路
		{
		       if(sh9_tms_dev.route_id ==7)  // 同和 to 体西
				dva_dev->play_list(362);
			else if(sh9_tms_dev.route_id ==1)  // 机场南/北 to 体西
				dva_dev->play_list(361);
			else
				dva_dev->play_list(300+sh9_tms_dev.next_station); 
		}
		else
			dva_dev->play_list(300+sh9_tms_dev.next_station); 
	}

	// 4. 站特殊信息
	if(sh9_tms_dev.next_station == 9) //广州塔
	{
	
		 if(sh9_tms_dev.route_id ==10)// 番禺 to 机场
		 {
			dva_dev->play_list(141);   
		 }
		 else if(sh9_tms_dev.route_id ==6) // 大石 to 同和
		 {
			dva_dev->play_list(141);   

		 }
		 else if(sh9_tms_dev.route_id ==14) // 番禺广场 to 同和
		 {
			dva_dev->play_list(141);   

		 }
		 else
		 {
              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
              else
                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
		 }
		

	}
	else if(sh9_tms_dev.next_station == 10) //珠江新城
	{

		 if(sh9_tms_dev.route_id ==10)// 番禺 to 机场
		 {
			dva_dev->play_list(151);   

		 }
		 else if(sh9_tms_dev.route_id ==6) // 大石 to 同和
		 {
			dva_dev->play_list(151);   

		 }
		 else if(sh9_tms_dev.route_id ==14) // 番禺广场 to 同和
		 {
			dva_dev->play_list(151);   

		 }
		 else
		 {
              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
              else
                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
		 }

	}
	
	
	else if(sh9_tms_dev.next_station == 11) //体育西路
	{
		if(sh9_tms_dev.next_station == sh9_tms_dev.end_station)
		{
			
			 if(sh9_tms_dev.route_id ==7)  // 同和 到 体西
				dva_dev->play_list(1163);
			
			 else if(sh9_tms_dev.route_id == 1)  // 机场南/北 到 体西
				dva_dev->play_list(1162);
			
			 else{
                              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
                              else
                                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
                       }		 
		}
		else
		{
			if(sh9_tms_dev.route_id ==3) // 天客 to 番禺
				dva_dev->play_list(1161);
                       if(sh9_tms_dev.route_id ==4) // 番禺 to 天客
                              dva_dev->play_list(161);
			else if(sh9_tms_dev.route_id ==5) // 同和 to 大石
				dva_dev->play_list(1164);
			else if(sh9_tms_dev.route_id ==6) // 大石 to 同和
				dva_dev->play_list(164);            
			else if(sh9_tms_dev.route_id ==9)// 机场 to 番禺
				dva_dev->play_list(1165);
			else if(sh9_tms_dev.route_id ==10)// 番禺 to 机场
				dva_dev->play_list(165);
			else if(sh9_tms_dev.route_id ==11)// 11.天客 to 大石
				dva_dev->play_list(1161);     //沿用天客 to 番禺
			else if(sh9_tms_dev.route_id ==12)// 12.大石 to 天客
				dva_dev->play_list(161);      //沿用番禺 to 天客

			
   			else{
                              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
                              else
                                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
                       }
		}
	}
	else if(sh9_tms_dev.next_station == 12) //林和西
	{
		if (sh9_tms_dev.route_id ==1)         // 机场南/北 to 体西
			dva_dev->play_list(1171);		
               else if(sh9_tms_dev.route_id ==2)  // 体西 to 机场南/北
			dva_dev->play_list(171);		
               else if(sh9_tms_dev.route_id ==5) // 同和 to 大石
			dva_dev->play_list(1172);		
               else if(sh9_tms_dev.route_id ==6) // 大石 to 同和
			dva_dev->play_list(172);		
               else if(sh9_tms_dev.route_id ==7) // 同和 to 体西
			dva_dev->play_list(1173);		
               else if(sh9_tms_dev.route_id ==8) // 体西 to 同和
			dva_dev->play_list(173);		                
		else{
                      if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                          dva_dev->play_list(100+sh9_tms_dev.next_station); 
                      else
                          dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
               }	
	}
	else if(sh9_tms_dev.next_station == 16)   // 京溪南方医院
	{	
		if(sh9_tms_dev.route_id ==8)  // 体西 到 同和
			dva_dev->play_list(191);
		else{
                      if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                          dva_dev->play_list(100+sh9_tms_dev.next_station); 
                      else
                          dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
               }			
	}
	else if(sh9_tms_dev.next_station == 17) //同和
	{
		if (sh9_tms_dev.route_id ==1)         // 机场南/北 to 体西
			dva_dev->play_list(1181);		
               else if(sh9_tms_dev.route_id ==2)  // 体西 to 机场南/北
			dva_dev->play_list(181);		
               else if(sh9_tms_dev.route_id ==6) // 大石 to 同和
			dva_dev->play_list(182);		
               else if(sh9_tms_dev.route_id ==8) // 体西 to 同和
			dva_dev->play_list(183);	
		else{
                      if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                          dva_dev->play_list(100+sh9_tms_dev.next_station); 
                      else
                          dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
               }	
	}
	else{
              if(sh9_tms_dev.start_station<sh9_tms_dev.end_station)  // 上行
                  dva_dev->play_list(100+sh9_tms_dev.next_station); 
              else
                  dva_dev->play_list(1100+sh9_tms_dev.next_station); // 下行
        }

	// 5. 终点站提示语
	if(sh9_tms_dev.next_station == sh9_tms_dev.end_station)
		dva_dev->play_list(601); 						
      				
	print_line("leave broadcast_proc");
}


//***********************************************************************************************************************
//函数作用:广播处理
//参数说明:无
//注意事项:
//返回说明:
//***********************************************************************************************************************
void broadcast_proc(void)
{
	uint8 channel;
	uint8 temp_priority[BROADCAST_NUM_MAX];//用于比较的临时优先级表
	uint8 broadcast_flag=0;//当前DVA广播标志
	uint8 special_broadcast_flag = 0;  //特殊广播标志
	uint8 dva_priority[3];
	//uint16 mp3_tmp = 0;
	uint8 tmp = 0;
	//uint8 station_id=0;
	uint8 i,j;

	routeid_handle();
	

	//-----------------------DVA广播处理---------------------------------
	//防止中途报站 影响提示音
	if(occ_req_times != 1 && dcp_req_times != 1)
	{

		//特殊广播--优先处理
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
		    if(!my_default_data.status_hbit.master)  //不是主不广播
			break;
			
		    //紧急广播
		    if((dva_priority[tmp] == BROADCAST_TYPE_SPECIAL) &&(special_broadcast_flag) 
				&& (broadcast_priority <= saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL]))
		    	{
		    	      if(tms_dev->special_broadcast_code != my_default_data.urgent_broadcast_no)
						{
					        my_default_data.urgent_broadcast_no = tms_dev->special_broadcast_code;//紧急广播代码
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
						//发送播放命令				
						dva_dev->dva_clear();						
						//dva_dev->play_special(tms_dev->special_broadcast_code+EMERGENCY_PA_BASE_ID);		
						dva_dev->play_special(tms_dev->special_broadcast_code);	
						print_line("special br");
						special_broadcast_times--;
						break;
		    	}
				
			//关门提示音
		    else if((dva_priority[tmp] == BROADCAST_TYPE_DOOR) &&(broadcast_flag == 3) 
				&& (broadcast_priority <= saveparam.broadcast_priority[BROADCAST_TYPE_DOOR]))
		    	{
		    	    //判断是否播放关门提示音
				if(saveparam.door_broadcast_flag)
				{
					close_door_playtimes=saveparam.close_door_playtimes;
					print_int("%x close:", close_door_playtimes);
				      dva_dev->dva_clear();  
					dva_dev->stop();
					 
					//---关门提示音播放触发
					if(saveparam.door_broadcast_flag)
					{
						//ptu_print_debug("d2");
						//apu_dev->set_door_out(0);
						close_door_times= get_clock();	
						apu_play_checktimes=get_clock();
						apu_play_status=1;
						apu_dev->set_door_out(1); //关门提示音从apu出
						
					}
					//如果没有播放 才能播放
					//if(!door_chime_dva)
					{
						//发送播放命令
						#if NAME_RULE_NEW
						//dva_dev->play_list(saveparam.dva_door_list[0]); //关门声音由APU播放
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
		    	    //判断是否播放关门提示音
				if(saveparam.door_broadcast_flag)
				{
				      dva_dev->dva_clear();
					//如果没有播放 才能播放
					//if(!door_chime_dva)
					{
						//发送播放命令
						#if NAME_RULE_NEW
						//dva_dev->play_list(saveparam.dva_door_list[0]); //关门声音由APU播放
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

			//报站广播
		    else if((dva_priority[tmp] == BROADCAST_TYPE_DVA) &&((broadcast_flag== 1) || (broadcast_flag== 2)) 
				&& (broadcast_priority <= saveparam.broadcast_priority[BROADCAST_TYPE_DVA]))
		    	{		    	    
		    	    	if( broadcast_flag == 1) //离站广播处理
		    	    	{
					leave_broadcast();
				}
				else if( broadcast_flag == 2 )//到站广播
				{
					//送标志
					my_default_data.trigger_hbit.leave_broadcast=0;
					my_default_data.trigger_hbit.arrived_broadcast=1;
					print_line("arrive broadcast");
				}
				break;
		    	}
			tmp++;
		}
	
		my_default_data.trigger_hbit.urgent_broadcast = (special_broadcast_times != 0);//送紧急广播标志
	}


	/*****apu 关门提示音播放处理*******/
	#if 1
	#if 1
	if( checktimer(&close_door_times,15000))//15s 没得到回复强制切断APU关门播放
	{
		close_door_playtimes=0;
		door_chime_dva = 0;
		close_times_flag = 0;
		close_door_times = get_clock();
		ptu_print_debug("10S");		
	}
	#endif
	if( checktimer(&apu_play_checktimes,1500))// 延时1s 读apu播放状态 
	{
		if (!apu_dev->get_door_in())
		{
			apu_play_status = 0;
		}
		else
			apu_play_status=1;
	}
	
	if (!apu_play_status) //apu 关门提示音播放完毕
	{
		if(close_door_playtimes==0) //播放次数为0 ，停止播放
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
				apu_dev->set_door_out(0);//拉高 为下面播放提供下降沿触发条件
				delay_n(5000);
				close_door_playtimes--;
				close_times_flag=1;
				close_door_times= get_clock();					
			}
			
		}
		else
			close_door_playtimes=0;	
		
		
		if(close_door_playtimes&&occ_req_times == 0)//关门提示音播放次数
		{
			if(close_door_playtimes>0)
			{
				door_chime_dva = 1;
				special_broadcast_dva = 0;
				station_broadcast_dva = 0;
				broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_DOOR];
				//---关门提示音播放触发
				if(saveparam.door_broadcast_flag)
				{
					//ptu_print_debug("d2");
					//apu_dev->set_door_out(0);
					if(close_times_flag)
					{
						apu_dev->set_door_out(1); //关门提示音从apu出
						apu_play_checktimes = get_clock();
						apu_play_status=1;
						print_line("close+");
						//print_line("d2");
					}
				}
				
			}
		}
		
	}
	else  //apu 关门提示音正在播放
	{
		close_times_flag=0;
		//ptu_print_debug("nd");
	}
	#endif

	/*****dva 播放处理*******/
	if(dva_dev->get_playing())//正在广播
	{
	      
		//TMS要求停止当前广播
		//DCP按取消广播按钮 
		if(sh9_tms_dev.emergency_status )//|| tms_dev->dva_stop)
		{
			//发送停止命令
			dva_dev->stop();
			sh9_tms_dev.emergency_status = 0;  //清停止特殊广播标志
			//清除特殊广播的次数
			special_broadcast_times=0;
			print_line("stop emg");
		}
	}
	else			//广播完毕
	{
	       if(sh9_tms_dev.emergency_status )//DCP停止紧急广播
		{
			//清除特殊广播的次数
			special_broadcast_times=0;
			sh9_tms_dev.emergency_status = 0;  //清停止特殊广播标志
		}
		   
		//判断是否还有特殊广播次数
		if(special_broadcast_times && occ_req_times == 0)
		{
			//发送播放命令
			if(special_broadcast_times>1)
			{
			      //for test
				//dva_dev->play_special(812);
				if(tms_dev->special_broadcast_code < 1)
					tms_dev->special_broadcast_code = 1;//防止下面的计算出现负数
				
				dva_dev->play_special(tms_dev->special_broadcast_code);
				
				//dva_dev->play_special(tms_dev->special_broadcast_code+9000);
				broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL];
				//dva_dev->play_special(901);
				special_broadcast_dva = 1;
				door_chime_dva = 0;
				station_broadcast_dva = 0;
				print_line("special br+");
			}
			//次数减少 并判断是否无限次数
			if(special_broadcast_times != 0xFF)
				special_broadcast_times--;
		}
		else
		{		
			//清除门广播
			//door_chime_dva=0;
			special_broadcast_dva= 0;
			station_broadcast_dva=0;

			broadcast_priority = 0;
			request_PA_times = 0;
		}
	}

	
	media_send_packet.status.emergency = 0;
	//------------------------广播通道优先级巡检和切换-------------------------
	//如果不是主 直接到没有任何广播
	if( !my_default_data.status_hbit.master )
		goto no_out;
	
	//送过程数据的广播优先级
	my_default_data.broadcast_priority.pecu = saveparam.broadcast_priority[BROADCAST_TYPE_ICC];
	my_default_data.broadcast_priority.talk = saveparam.broadcast_priority[BROADCAST_TYPE_TALK];
	my_default_data.broadcast_priority.manual = saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL];
	my_default_data.broadcast_priority.occ = saveparam.broadcast_priority[BROADCAST_TYPE_OCC];
	my_default_data.broadcast_priority.media = saveparam.broadcast_priority[BROADCAST_TYPE_MEDIA];
	my_default_data.broadcast_priority.dva = saveparam.broadcast_priority[BROADCAST_TYPE_DVA];
	my_default_data.broadcast_priority.door = saveparam.broadcast_priority[BROADCAST_TYPE_DOOR];
	my_default_data.broadcast_priority.special = saveparam.broadcast_priority[BROADCAST_TYPE_SPECIAL];
	//把参数拷贝到临时优先级表
	memcpy(temp_priority,(uint8 *)&saveparam.broadcast_priority, BROADCAST_NUM_MAX);
	//按照优先级遍历所有广播通道

	for(channel=0; channel<BROADCAST_NUM_MAX; channel++)
	{
		uint8 broadcast_index;//通道循环变量
		uint8 highest_priority = 0;//下一个最高优先级
		uint8 highest_index = 0xFF;//最高优先级的索引

		
		//查找下一个高优先级
		for(broadcast_index=0;broadcast_index<BROADCAST_NUM_MAX;broadcast_index++)
		{
			//把上次循环处理过的通道排除
			if(temp_priority[broadcast_index])
			{
				//比较产生最大值
				if(temp_priority[broadcast_index]>=highest_priority)
				{
					highest_priority=temp_priority[broadcast_index];
					highest_index=broadcast_index;
				}
			}
		}
		
		//如果找到下一个高优先级 分类处理找到通道的广播
		if(highest_index<BROADCAST_NUM_MAX)
		{			
			//清除临时优先级表中相应的数
			temp_priority[highest_index]=0;
					
			//OCC		判断OCC监测线,  OCC优先级大于等于当前优先级
			if((highest_index==BROADCAST_TYPE_OCC) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{     
				if(apu_dev->get_occ_in())
				{
					dcp_req_times=0;					
					
					//判断是否发送请求
					//pa占用并且为dcp占用，或者dcp正在广播状态
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_OCC;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//信源指示灯
						led_blink(BROADCAST_OCC_LED_NO,250,250);
						led_onoff(BROADCAST_DVA_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						request_PA_times++;
					}
					//判断 直接切换
					else
					{
					       occ_req_times = 2;

					#if 0     //取消提示音
						//第一次申请播放提示音
						if(occ_req_times==0)
						{
							//OCC广播提示音
							uint16 occ_pre_sound[]={800,0};
							
							occ_req_times=1;
							//播放提示音
							#if NAME_RULE_NEW
							dva_dev->play_list(occ_pre_sound[0]);
							#else
							dva_dev->play_list(occ_pre_sound);
							#endif
							
							print_line("OCC broadcast");
							//送过程数据
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
							my_default_data.broadcast_signal.broadcast_req=0;
							my_default_data.broadcast_signal.broadcasting=1;
							//切换通道
							apu_dev->set_channel(APU_CHANNEL_DVA);
							//占用PA线
							if(!apu_dev->get_pa_out())
							       apu_dev->set_pa_out(1);
							//信源指示灯
							led_onoff(BROADCAST_OCC_LED_NO,1);
							led_onoff(BROADCAST_DVA_LED_NO,0);
							led_onoff(BROADCAST_MEDIA_LED_NO,1);
						}
						//不是第一次申请 等待播放完毕
						else if(occ_req_times==1)
						{
							//播放完毕 清过程数据 释放PA
							if(!dva_dev->get_playing())
							{
								occ_req_times=2;
								
							}
							
						}
					#endif
						if(occ_req_times==2)
						{				
							//送过程数据
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_OCC;
							my_default_data.broadcast_signal.broadcast_req=0;
							my_default_data.broadcast_signal.broadcasting=1;
							//占用PA线
							if(!apu_dev->get_pa_out())
							       apu_dev->set_pa_out(1);
							//切换通道
							apu_dev->set_channel(APU_CHANNEL_OCC);
							//信源指示灯
							led_onoff(BROADCAST_OCC_LED_NO,0);
							led_onoff(BROADCAST_DVA_LED_NO,1);
							led_onoff(BROADCAST_MEDIA_LED_NO,1);
							
						}
					}
					//当前为OCC广播
					broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_OCC];
					// should delete
				        //broadcast_priority = 0;
					//debug_print_line("occ return");
					
					return;
				}
			
				else if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_OCC])
				{
				    //当前优先级为occ广播，但已播完occ广播，清零当前广播优先级
				    broadcast_priority = 0;
				   occ_req_times = 0;
				}

                            // should delete
				//broadcast_priority = 0;

			}

			/***************广3单条音频总线  CC  PC************************/

			// CC 对讲  中断报站
			else if((highest_index==BROADCAST_TYPE_TALK) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{
				if(dcp_get_talk_state()) //有对讲
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

			// PC 对讲  中断报站
			else if((highest_index==BROADCAST_TYPE_ICC) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{
				if(dcp_get_emtalking_state()) //有对讲
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
		
			//DCP		判断DCP请求和广播标志,人工广播优先级大于等于当前优先级
			else if((highest_index==BROADCAST_TYPE_MANUAL) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{
			     //  uart0_write_buf((uint8*)&dcp_default_data[0], sizeof(dcp_default_data_t));
			      // uart0_write_char(dcp_default_data[0].broadcast_hbit.broadcast_req);
				//   uart0_write_char(dcp_get_broadcast_state());
				//人工广播申请 播放提示音
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
					//第一次申请播放提示音
					if(dcp_req_times==0)
					{
						//人工广播提示音
						dva_dev->dva_clear();
						dcp_req_times=2;
						//dcp_req_times=1;
						//播放提示音
						//dva_dev->play_list(233);
						//dva_dev->play_list(501);
						
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//切换通道
						apu_dev->set_channel(APU_CHANNEL_DVA);
						//占用PA线
						if(!apu_dev->get_pa_out())
						     apu_dev->set_pa_out(1);
						//信源指示灯
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_DVA_LED_NO,0);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
					}
					//不是第一次申请 等待播放完毕
					else if(dcp_req_times==1)
					{
						//播放完毕 清过程数据 释放PA
						if(!dva_dev->get_playing())
						{
							dcp_req_times=2;
						}
					}
					#endif
					if(dcp_req_times==2)
					{
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_NONE;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=0;					
						//切换通道
						apu_dev->set_channel(APU_CHANNEL_NONE);
						//释放PA线

						//if(apu_dev->get_pa_out())
              					apu_dev->set_pa_out(0);
						//信源指示灯
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_DVA_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
					}

					 //dcp人工广播
                                 broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL];

					//debug_print_line("P1");
					//退出循环
					return;						
				}
				//正在人工广播 直接切换通道
				else if(dcp_get_broadcast_state())
				{
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					special_broadcast_dva= 0;
					station_broadcast_dva=0;
					special_broadcast_times = 0;
					
					//送过程数据
					my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_NONE;
					my_default_data.broadcast_signal.broadcast_req=0;
					my_default_data.broadcast_signal.broadcasting=0;				
					//切换通道
					apu_dev->set_channel(APU_CHANNEL_NONE);
					//释放PA线
                                if(apu_dev->get_pa_out())
					    apu_dev->set_pa_out(0);
					//信源指示灯
					led_onoff(BROADCAST_OCC_LED_NO,1);
					led_onoff(BROADCAST_DVA_LED_NO,1);
					led_onoff(BROADCAST_MEDIA_LED_NO,1);

					//dcp人工广播
                                 broadcast_priority = saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL];

					//debug_print_line("P2");
					//退出循环
					return;
				}
				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_MANUAL])
				{	
				    broadcast_priority = 0;
				}
			}
			//关门提示音	判断TMS发送的关门提示音,关门提示音优先级大于等于当前优先级
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
					
					//判断是否发送请求
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//送过程数据
						if(saveparam.door_broadcast_flag)
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DOOR;
						else
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						
						//信源指示灯
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
					//判断 直接切换
					else
					{
						//送过程数据
						if(saveparam.door_broadcast_flag)
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DOOR;
						else
							my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;					
						//占用PA线
						if(!apu_dev->get_pa_out())
							apu_dev->set_pa_out(1);
						//---关门提示音播放触发
						//切换通道
						apu_dev->set_channel(APU_CHANNEL_NONE);
						#if 0
						if(saveparam.door_broadcast_flag)
						{
							ptu_print_debug("d2");
							apu_dev->set_door_out(0);
							apu_dev->set_door_out(1); //关门提示音从apu出
							
						}
						#endif
						//信源指示灯
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
				//当前为关门提示音，但已播完，广播优先级清零
				if(broadcast_priority == saveparam.broadcast_priority[BROADCAST_TYPE_DOOR])
					broadcast_priority = 0;
			}
			//DVA		判断DVA监测线
			else if((highest_index==BROADCAST_TYPE_DVA) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{				
				if(station_broadcast_dva)
				{
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					special_broadcast_dva = 0;
					special_broadcast_times = 0;
					
					//判断是否发送请求
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//信源指示灯
						led_blink(BROADCAST_DVA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						debug_print_line("3");
					}
					//判断 直接切换
					else
					{
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//占用PA线
						if(!apu_dev->get_pa_out())
						    apu_dev->set_pa_out(1);
						//切换通道
						//判断是否同时播放背景音乐
						if(saveparam.media_sametime)
						{
							apu_dev->set_channel(APU_CHANNEL_DVA_MEDIA);//切换通道
							//debug_print_line("AP1");
						}
						else
						{
							apu_dev->set_channel(APU_CHANNEL_DVA);//切换通道
							//debug_print_line("AP2");
						}
						//信源指示灯
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

			//紧急广播
			else if((highest_index==BROADCAST_TYPE_SPECIAL) && (broadcast_priority <= saveparam.broadcast_priority[highest_index]))
			{				
				if(special_broadcast_times>=1)
				{
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					station_broadcast_dva = 0;
					
					//判断是否发送请求
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_SPECIAL;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//信源指示灯
						led_blink(BROADCAST_DVA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						debug_print_line("3");
					}
					//判断 直接切换
					else
					{
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_SPECIAL;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//占用PA线
						if(!apu_dev->get_pa_out())
						    apu_dev->set_pa_out(1);
						//切换通道
						//判断是否同时播放背景音乐
						if(saveparam.media_sametime)
						{
							apu_dev->set_channel(APU_CHANNEL_DVA_MEDIA);//切换通道
							//debug_print_line("AP1");
						}
						else
						{
							apu_dev->set_channel(APU_CHANNEL_DVA);//切换通道
							//debug_print_line("AP2");
						}
						//信源指示灯
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
				if(apu_dev->get_test_in()) //有测试输入
				{
					ptu_print_debug("test");
					if(!dva_dev->get_playing())
						dva_dev->play_list(1000);
					dcp_req_times=0;
					occ_req_times=0;
					door_chime_dva = 0;
					special_broadcast_dva = 0;
					special_broadcast_times = 0;
					
					//判断是否发送请求
					if((request_PA_times < REQUEST_PA_TIMES) &&((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state()))
					{
					      request_PA_times++;
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//信源指示灯
						led_blink(BROADCAST_DVA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_MEDIA_LED_NO,1);
						debug_print_line("3");
					}
					//判断 直接切换
					else
					{
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_DVA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;
						//占用PA线
						if(!apu_dev->get_pa_out())
						    apu_dev->set_pa_out(1);
						//切换通道
						//判断是否同时播放背景音乐
						if(saveparam.media_sametime)
						{
							apu_dev->set_channel(APU_CHANNEL_DVA_MEDIA);//切换通道
							//debug_print_line("AP1");
						}
						else
						{
							apu_dev->set_channel(APU_CHANNEL_DVA);//切换通道
							//debug_print_line("AP2");
						}
						//信源指示灯
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
			//MEDIA		默认有媒体伴音并且当前没有重连
			else if((highest_index==BROADCAST_TYPE_MEDIA)&&(LINK_CTRL_REG&(1<<LINK_CTRL_BIT))
				&& !other_default_data.test_status.reconnection)
			//if(0)
			{
				if(!my_default_data.status_hbit.active 
					&& !other_default_data.status_hbit.active
					&& !dcp_default_data[0].status_hbit.active
					&& !dcp_default_data[1].status_hbit.active) //两端没钥匙不播媒体
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
					
					//判断是否发送请求
					if((apu_dev->get_pa_in() && !apu_dev->get_pa_out()) || dcp_get_broadcast_state())
					{
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=1;
						my_default_data.broadcast_signal.broadcasting=0;
						//信源指示灯
						led_blink(BROADCAST_MEDIA_LED_NO,250,250);
						led_onoff(BROADCAST_OCC_LED_NO,1);
						led_onoff(BROADCAST_DVA_LED_NO,1);
					}
					//判断 直接切换
					else
					{
						//送过程数据
						my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_MEDIA;
						my_default_data.broadcast_signal.broadcast_req=0;
						my_default_data.broadcast_signal.broadcasting=1;					
						//占用PA线
						if(!apu_dev->get_pa_out())
					          apu_dev->set_pa_out(1);
						//切换通道
						apu_dev->set_channel(APU_CHANNEL_MEDIA);
						//信源指示灯
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


//没有任何广播
no_out:
	//door_pa_status = 0;  //for test open door PA
	//tms_dev->set_broadcast(0);//for test open door PA
	dcp_req_times=0;
	occ_req_times=0;
	door_chime_dva = 0;
	special_broadcast_dva= 0;
	station_broadcast_dva=0;
	broadcast_priority = 0;
	//没有任何通道得到PA控制权 送过程数据广播类型
	my_default_data.broadcast_signal.broadcast_type=PISC_BROADCAST_NONE;
	apu_dev->set_channel(APU_CHANNEL_NONE);//切换通道
	
         //if(apu_dev->get_pa_out())
	    apu_dev->set_pa_out(0);//释放PA线
	my_default_data.broadcast_signal.broadcast_req=0;
	my_default_data.broadcast_signal.broadcasting=0;
	//信源指示灯
	led_onoff(BROADCAST_OCC_LED_NO,1);
	led_onoff(BROADCAST_DVA_LED_NO,1);
	led_onoff(BROADCAST_MEDIA_LED_NO,1);	

}


