#include "stm32f2xx.h"
#include "Uart1.h"
#include "Usb.h"
#include "include.h"
#include "TypeDef.h"
#include "at45db161.h"

extern void UpdateFunc(u8 *buf, u16 lenth);

extern uint8_t gDownFirmware;


#define UART1                       0x01
static u8 com1_recv_buf[512];
u8 com1_recv_length = 0;



static u8 uart1_get_packet(void);

void com_send_data(u8 *buf, u8 length, u8 uartx)
{
    u8 *send_buf;
    u8 send_length = 0;
    u8 checksum = 0;
    u8 cnt = 0;
    u8 *ptr;
    send_buf = uart1_send_buf;
    send_buf[send_length++] = 0x7e;
    ptr = (u8 *)buf;
    
    for (cnt = 0; cnt < length; cnt++)
    {
        if (*ptr == 0x7e)
        {
            send_buf[send_length++] = 0x7f;
            send_buf[send_length++] = 0x80;
        }
        else if (*ptr == 0x7f)
        {
            send_buf[send_length++] = 0x7f;
            send_buf[send_length++] = 0x81;
        }
        else
        {
            send_buf[send_length++] = *ptr;
        }
        
        checksum += *ptr;
        ptr++;
    }
    
    checksum = 0x55 - checksum;
    
    //校验和
    if (checksum == 0x7e)
    {
        send_buf[send_length++] = 0x7f;
        send_buf[send_length++] = 0x80;
    }
    else if (checksum == 0x7f)
    {
        send_buf[send_length++] = 0x7f;
        send_buf[send_length++] = 0x81;
    }
    else
    {
        send_buf[send_length++] = checksum;
    }
    
    //send_buf[send_length++] = checksum;  //校验和
    send_buf[send_length++] = 0x7e;
    uart1_write_buf((u8 *)uart1_send_buf, send_length);
}



static u8 gOtherSystemFlag = 0;
static u8 myBuf[256];
static u8 myLen;
static u8 otherBuf[32];
static u8 otherLen;

static u8 uart1_get_packet_1(void)
{
    u8 temp;
    static u8 mySys = 0;
    static u8 otherSys = 0;
    static u32 tick;
    com1_recv_length = 0;
    
    while (uart1_read_char(&temp))
    {
        tick = gSystemTick;
        
        if (temp == TBA_PACKET_TAB)
        {
            //判断长度 判断是否有数据
            if (myLen >= 3)
            {
                memcpy(com1_recv_buf, myBuf, myLen);
                com1_recv_length = myLen;
                gOtherSystemFlag = 0;
                myLen = 0;
                otherLen = 0;
                return 1;
            }
            
            myLen = 0;
        }
        //其他字符串直接送缓冲区
        else
        {
            myBuf[myLen] = temp;
            
            if ( ++myLen >= 256 )
                myLen = 0;
        }
        
#if CHOISE_PTU
        
        if (temp == 0xff)
        {
            otherSys = 1;
            
            if (otherLen > 3)
            {
                otherBuf[otherLen] = temp;  //这个0xff可能是校验和
                
                if ( ++otherLen >= 32 )
                {
                    otherLen = 0;
                }
                
                continue;
            }
            else otherLen = 0;
        }
        else if (temp == 0xfe)
        {
            //判断长度 判断是否有数据
            if ((otherLen == 14) && (otherSys == 1))
                //校验和可能是0xfe
            {
                otherBuf[otherLen] = temp;
                
                if ( ++otherLen >= 32 )
                {
                    otherLen = 0;
                }
            }
            else if ((otherLen == 15) && (otherSys == 1) && (otherBuf[6] == 0x07))
            {
                memcpy(com1_recv_buf, otherBuf, otherLen);
                com1_recv_length = otherLen;
                gOtherSystemFlag = 1;
                myLen = 0;
                otherLen = 0;
                return 1;
            }
            else if (otherBuf[1] != 0x05  && otherBuf[2] != 0xfd)
            {
                otherSys = 0;
                otherLen = 0;
            }
        }
        //其他字符串直接送缓冲区
        else
        {
            otherBuf[otherLen] = temp;
            
            if ( ++otherLen >= 32 )
            {
                otherLen = 0;
            }
        }
        
#endif
    }
    
    if (gSystemTick - tick > 20)
    {
        tick = gSystemTick;
        otherLen = 0;
        myLen = 0;
    }
    
    return 0;
}
#if 0
{
    u8 temp;
    static u8 mySys = 0;
    static u8 otherSys = 0;
    static u32 tick;
    
    while (uart1_read_char(&temp))
    {
        tick = gSystemTick;
        
        if (temp == TBA_PACKET_TAB)
        {
            //判断长度 判断是否有数据
            if (myLen >= 3)
            {
                memcpy(com1_recv_buf, myBuf, myLen);
                com1_recv_length = myLen;
                gOtherSystemFlag = 0;
                myLen = 0;
                otherLen = 0;
                return 1;
            }
            
            myLen = 0;
        }
        //其他字符串直接送缓冲区
        else
        {
            myBuf[myLen] = temp;
            
            if ( ++myLen >= 256 )
                myLen = 0;
        }
        
        if (temp == 0xff)
        {
            otherSys = 1;
            
            //这个0xff可能是校验和
            if (otherLen > 3) continue;
            else otherLen = 0;
        }
        else if (temp == 0xfe)
        {
            //判断长度 判断是否有数据
            if ((otherLen == 14) && (otherSys == 1)) //校验和可能是0xfe
            {
                otherBuf[otherLen] = temp;
                
                if ( ++otherLen >= 32 )
                {
                    otherLen = 0;
                }
            }
            else if ((otherLen == 15) && (otherSys == 1) && (otherBuf[6] == 0x07))
            {
                memcpy(com1_recv_buf, otherBuf, otherLen);
                com1_recv_length = otherLen;
                gOtherSystemFlag = 1;
                myLen = 0;
                otherLen = 0;
                return 1;
            }
        }
        //其他字符串直接送缓冲区
        else
        {
            otherBuf[otherLen] = temp;
            
            if ( ++otherLen >= 32 )
            {
                otherLen = 0;
            }
        }
    }
    
    if (gSystemTick - tick > 20)
    {
        tick = gSystemTick;
        otherLen = 0;
        myLen = 0;
    }
    
    return 0;
}
#endif

static u8 uart1_get_packet_2(void)
{
    u8 temp;
    
    while (uart1_read_char(&temp))
    {
        if (temp == TBA_PACKET_TAB)
        {
            //判断长度 判断是否有数据
            if (com1_recv_length >= 3)
            {
                return 1;
            }
            
            com1_recv_length = 0;
        }
        //其他字符串直接送缓冲区
        else
        {
            com1_recv_buf[com1_recv_length] = temp;
            
            if ( ++com1_recv_length >= 512 )
                com1_recv_length = 0;
        }
    }
    
    return 0;
}


void com_send_data_other_systerm(u8 *buf, u8 length, u8 uartx)
{
    u8 *send_buf;
    u8 send_length = 0;
    u8 checksum = 0;
    u8 cnt = 0;
    u8 *ptr;
    send_buf = uart1_send_buf;
    send_buf[send_length++] = 0xff;
    ptr = (u8 *)buf;
    
    for (cnt = 0; cnt < length; cnt++)
    {
        send_buf[send_length++] = *ptr;
        checksum += *ptr;
        ptr++;
    }
    
    send_buf[send_length++] = 0xfe;
    uart1_write_buf((u8 *)uart1_send_buf, send_length);
}


void com1_proc_other_system(void)
{
    u8 buf[30] = {0x03, 0xff, 0x05, 0xee, 0x02, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    tFrameHeaderOld *frame_header;
    u8 recv_flag = 0;
    u8 checksum = 0;
    u8 i;
    frame_header = (tFrameHeaderOld *)com1_recv_buf;
    
    if (frame_header->frameLength == 7)
    {
        checksum = 0;
        
        for (i = 0; i < 14; i++)
        {
            checksum += com1_recv_buf[i];
        }
        
        if (checksum == com1_recv_buf[14])
        {
            recv_flag = 1;
        }
        else
        {
            recv_flag = 0;
        }
    }
    else
    {
        recv_flag = 0;
    }
    
    com1_recv_length = 0;
    
    if (recv_flag)
    {
        gScrSavTick = 0;
        frame_header = (tFrameHeaderOld *)com1_recv_buf;
        
        if (frame_header->receiverId == 0xfd)
        {
            if (frame_header->transmitType == 0x03)
            {
                ProcessDataOperat(com1_recv_buf);
            }
        }
        else if (frame_header->receiverId == gDeviceId)
        {
            if (frame_header->tokenControl == 0x01)
            {
                buf[1] = frame_header->transmitId;
                buf[3] = gDeviceId;
                com_send_data_other_systerm(buf, 14, 0);
            }
        }
    }
}




extern u8 FIRST ;
extern u32 gScrSavTime;
void com1_proc(void)
{
#if IDU_COMPILE
    u8 buf[30] = {0x00, 0x03, 0x00, 0xee, 0x10, 0x02, 0xee, 0xee};
#endif
#if SDU_COMPILE
    u8 buf[30] = {0x00, 0x03, 0x00, 0xee, 0x10, 0x02, 0xee, 0xee};
#endif
#if FDU_COMPILE
    u8 buf[30] = {0x00, 0xff, 0x00, 0xee, 0x10, 0x02, 0xee, 0xee};
#endif
    tFrameHeader *frame_header;
    u8 recv_flag = 0;
    u8 checksum = 0;
    u16 src_length = 0; //逆转码之前的长度
    u16 dst_length = 0; //逆转码以后的长度
    
    if (uart1_get_packet_1()) //处理协议里面包含了FF  7E的处理
    {
        if (gOtherSystemFlag == 1)
        {
            gOtherSystemFlag = 0;
            com1_proc_other_system();
            return;
        }
        
        //执行逆转码 并计算校验和
        for (src_length = 0; src_length < com1_recv_length; src_length++)
        {
            //拷贝数据
            com1_recv_buf[dst_length] = com1_recv_buf[src_length];
            
            //判断刚刚拷贝的是否是转码符
            if (com1_recv_buf[src_length] == TBA_PACKET_DLE && src_length < com1_recv_length - 1)
            {
                //判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_TAB 并跳过下一个字符
                if (com1_recv_buf[src_length + 1] == TBA_PACKET_DLE_TAB)
                {
                    com1_recv_buf[dst_length] = TBA_PACKET_TAB; //把刚刚拷贝的转码符还原为TMS_PACKET_TAB
                    src_length++;//并跳过下一个字符
                }
                //判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_DLE 并跳过下一个字符
                else if (com1_recv_buf[src_length + 1] == TBA_PACKET_DLE_DLE)
                {
                    com1_recv_buf[dst_length] = TBA_PACKET_DLE; //把刚刚拷贝的转码符还原为TMS_PACKET_DLE
                    src_length++;//并跳过下一个字符
                }
                else
                {
                    //print_line("tms 7f error");
                    //数据错误----7f没有被正确转码
                    dst_length = 0; //清除转码以后的长度
                    break;
                }
            }
            
            checksum += com1_recv_buf[dst_length];
            dst_length++;
        }
        
        if (checksum == 0x55)
        {
            recv_flag = 1;
        }
        else
        {
            recv_flag = 0;
        }
    }
    
    if (recv_flag)
    {
        gScrSavTime = 0;
        frame_header = (tFrameHeader *)com1_recv_buf;          //收到串口发来的数据太长
        
        switch ((frame_header->cmd) & 0x0f)
        {
            case PROCESS_PACKET:
                if (gDownFirmware == 1) break;
                
                if ((frame_header->src_eqnum.eq_type == PISC) && (frame_header->data_.status_hbit.master))
                {
                    gScrSavTick = 0;
                    FIRST = 0;
                    ProcessDataOperat(com1_recv_buf);
                }
                
                break;
                
            case TOKEN_PACKET:
                if (gDownFirmware == 1) break;
                
                if (frame_header->dest_eqnum.eq_type == LOCAL_DEVICE_TYPE)
                {
                    if (frame_header->dest_eqnum.eq_num != gDeviceId) break;
                    
                    buf[3] = gDeviceId;
                    buf[3] <<= 4;
                    buf[3] |= LOCAL_DEVICE_TYPE;
                    buf[6] = MSTER_VERSION;
                    buf[6] <<= 4;
                    buf[6] |= SLAVE_VERSION;
                    
                    if (gBmpUpdateFlag == BMP_OK) buf[7] = 1;
                    else buf[7] = 0;
                    
                    com_send_data(buf, 8, 0);
                }
                
                break;
                
            case DOWNLOAD_PACKET:
                if (frame_header->dest_eqnum.eq_type == LOCAL_DEVICE_TYPE)
                {
                    UpdateFunc(&com1_recv_buf[6], frame_header->len);
                }
                
                break;
        }
    }
    
#if DEBUG_ALL_LIGHT_ON
    
    if (gDispStat.disp_type == DISP_TYPE_TEST_ALL_LIGHT) gScrSavTick = 0;
    
    // 10 minute
    if (gScrSavTick > ((10 * 1000000 * 10 / 18) / SYS_TICK_TIME) * 60) //((10 * 60 * 1000000)/SYS_TICK_TIME)
    {
        if (gDispStat.disp_type != DISP_TYPE_DOWNING)
        {
            DispTypeInit(DISP_TYPE_NO_PROCESS);
            gScrSavTick = 0;
            gOldTerminal = 0xff;
        }
    }
    
#endif
    /*
        //分钟转换为10ms base
        if (gSystemTick - gDispStat.scr_sav_tick > (10 * 60 * 100)) {
            if (gDispStat.disp_type != DISP_TYPE_DOWNING) DispTypeInit(DISP_TYPE_NO_PROCESS);
        }
    */
}



