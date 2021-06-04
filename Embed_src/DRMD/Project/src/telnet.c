/**
  ******************************************************************************
  * @file    helloworld.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   A hello world example based on a Telnet connection
  *          The application works as a server which wait for the client request
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "telnet.h"
#include "config.h"
#include "lwip/tcp.h"
#include <string.h>
#include "flash_if.h"
#include "sd_data.h"
#include "lwip/udp.h"
#include <stdio.h>
#include <stdlib.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define HELLO "Hello "
#define MAX_NAME_SIZE 128
#define NET_STAT_TITLE "Proto             Local Address             Foreign Address         State\r\n"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct name 
{
  int length;
  char bytes[MAX_NAME_SIZE];
};
struct tcp_pcb *pcb23 = NULL;
char tmp_buf[256];

/* Private function prototypes -----------------------------------------------*/
static err_t telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err);
static void telnet_conn_err(void *arg, err_t err);
void ip_config(char *buf);
void showip(void);
void get_data(char *buf);
void set_date(char *buf);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Called when a data is received on the telnet connection
  * @param  arg	the user argument
  * @param  pcb	the tcp_pcb that has received the data
  * @param  p	the packet buffer
  * @param  err	the error value linked with the received data
  * @retval error value
  */
static err_t telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  struct pbuf *q;
  struct name *name = (struct name *)arg;
  int done;
  char *c;
  int i;
 

  /* We perform here any necessary processing on the pbuf */
  if (p != NULL) 
  {        
	/* We call this function to tell the LwIp that we have processed the data */
	/* This lets the stack advertise a larger window, so more data can be received*/
	tcp_recved(pcb, p->tot_len);

    /* Check the name if NULL, no data passed, return withh illegal argument error */
	if(!name) 
    {
      pbuf_free(p);
      return ERR_ARG;
    }

    done = 0;
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done; i++) 
	  {
        done = ((c[i] == '\r') || (c[i] == '\n'));
        if(name->length < MAX_NAME_SIZE) 
	    {
          name->bytes[name->length++] = c[i];
        }
      }
    }
    if(done) 
    {
  #if 1
      if(name->bytes[name->length-2] != '\r' || name->bytes[name->length-1] != '\n') 
	  {
        if((name->bytes[name->length-1] == '\r' || name->bytes[name->length-1] == '\n') && (name->length+1 <= MAX_NAME_SIZE)) 
	    {
	      name->length += 1;
        } 
	    else if(name->length+2 <= MAX_NAME_SIZE) 
	    {
          name->length += 2;
        } 
	    else 
	    {
          name->length = MAX_NAME_SIZE;
        }

        name->bytes[name->length-2] = '\r';
        //name->bytes[name->length-1] = '\n';
      }
      name->bytes[name->length-1] = '\0';
   #endif
      //tcp_write(pcb, HELLO, strlen(HELLO), 1);
      parse_input_parm(name->bytes, name->length);
      //tcp_write(pcb, name->bytes, name->length, TCP_WRITE_FLAG_COPY);
      memset(name->bytes, 0x00, sizeof(name->bytes));
      name->length = 0;
    }
	//i = name->length;
	/* End of processing, we free the pbuf */
    pbuf_free(p);
  }  
  else if (err == ERR_OK) 
  {
    /* When the pbuf is NULL and the err is ERR_OK, the remote end is closing the connection. */
    /* We free the allocated memory and we close the connection */
    mem_free(name);
    return tcp_close(pcb);
  }
  return ERR_OK;


}

/**
  * @brief  This function when the Telnet connection is established
  * @param  arg  user supplied argument 
  * @param  pcb	 the tcp_pcb which accepted the connection
  * @param  err	 error value
  * @retval ERR_OK
  */
static err_t telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{     
  char buf[64] = {0};
  /* Tell LwIP to associate this structure with this connection. */
  tcp_arg(pcb, mem_calloc(sizeof(struct name), 1));	
  
  /* Configure LwIP to use our call back functions. */
  tcp_err(pcb, telnet_conn_err);
  tcp_recv(pcb, telnet_recv);
  
  /* Send out the first message */  
  strncpy(buf, GREETING, strlen(GREETING));
  strncat(buf, VERSION, sizeof(buf)- strlen(GREETING));
  tcp_write(pcb, buf, strlen(buf), 1); 
  //tcp_write(pcb, GREETING, strlen(GREETING), 1); 
  tcp_output(pcb);
  if(pcb23 != NULL)
  {
      mem_free((struct name*)pcb23->callback_arg);
      //tcp_close(pcb23);
      tcp_abort(pcb23);
  }
  pcb23 = pcb;
  return ERR_OK;
}

/**
  * @brief  Initialize the hello application  
  * @param  None 
  * @retval None 
  */
void telnet_init(void)
{
  struct tcp_pcb *pcb;	            		
  
  /* Create a new TCP control block  */
  pcb = tcp_new();	                		 	

  /* Assign to the new pcb a local IP address and a port number */
  /* Using IP_ADDR_ANY allow the pcb to be used by any local interface */
  tcp_bind(pcb, IP_ADDR_ANY, 23);       


  /* Set the connection to the LISTEN state */
  pcb = tcp_listen(pcb);				

  /* Specify the function to be called when a connection is established */	
  tcp_accept(pcb, telnet_accept);   
										
}

/**
  * @brief  This function is called when an error occurs on the connection 
  * @param  arg
  * @parm   err
  * @retval None 
  */
static void telnet_conn_err(void *arg, err_t err)
{
  struct name *name;
  name = (struct name *)arg;

  mem_free(name);
}


void send_msg_to_telnet(unsigned char *buf, int length)
{
    if(NULL != pcb23)
   {
	tcp_write(pcb23, buf, length, TCP_WRITE_FLAG_COPY);
	tcp_write(pcb23, "\r\n", 2, TCP_WRITE_FLAG_COPY);
    }
}
int int_to_buf(char*buf, unsigned char *src, int length);
void send_int_buf_to_telnet(unsigned char *buf, int length)
{
#if 1
    uint8_t asiic_buf[]="0123456789ABCDEF";
    int i=0;
    int index = 0;
    if(NULL != pcb23)
   {
	for(i = 0; i < length; i++)
	{
	    if(index >= 255)
	    {
		  tcp_write(pcb23, tmp_buf, index, TCP_WRITE_FLAG_COPY);
		  index = 0;
	    }
	    tmp_buf[index++] = asiic_buf[buf[i]>>4];
	    tmp_buf[index++] = asiic_buf[buf[i]&0x0F];
	    tmp_buf[index++] = ' ';
	}
 	tcp_write(pcb23, tmp_buf, index, TCP_WRITE_FLAG_COPY);
	tcp_write(pcb23, "\r\n", 2, TCP_WRITE_FLAG_COPY);
	tcp_output(pcb23);
    }
#endif
}

 void parse_input_parm(char *buf, int length)
{
    char *p = strstr(buf, "ifconfig");
    if(p)
    {
        ip_config(p);
    }
    p = strstr(buf, "default");
    if(p)
    {
       load_default_parm();
    }
    p = strstr(buf, "date");
    if(p)
    {
        set_date(p);
    }
}

void set_date(char *buf)
{
    int year, mon, date, hour, min, sec;
    char temp_buf[24] = {0};
    RTC_TimeTypeDef  rtc_time;
    RTC_DateTypeDef  rtc_date;
    if(strlen(buf) <= 6)
    {
        RTC_GetTime(RTC_Format_BIN, &rtc_time);
        RTC_GetDate(RTC_Format_BIN, &rtc_date);
	  sprintf(temp_buf, "%d-%d-%d %d:%d:%d\r\n", rtc_date.RTC_Year+2000, rtc_date.RTC_Month, rtc_date.RTC_Date, rtc_time.RTC_Hours,
	  	rtc_time.RTC_Minutes, rtc_time.RTC_Seconds);
	  tcp_write(pcb23,temp_buf, strlen(temp_buf), TCP_WRITE_FLAG_COPY);
	  return;
    }
    sscanf(buf, "%s %d-%d-%d %d:%d:%d", temp_buf, &year, &mon, &date, &hour, &min, &sec);
    if(year <= 0 || year > 2099 || mon <= 0 || mon > 12 || date <= 0 || date > 31 || hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59)
    {
        tcp_write(pcb23, "invalid parameter\r\n", strlen("invalid parameter\r\n"), TCP_WRITE_FLAG_COPY);
	  return;
    }
    set_rtc_date(year%2000, mon, date);
    set_rtc_time(hour, min, sec);
}
#if 0
void get_data(char *buf)
{
    index_data_t from;
    index_data_t to;
    char temp_buf[16] = {0};
    //uint8_t i = 0;
    //uint8_t flag = 0;
    int from_year, to_year;
    sscanf(buf, "%s %d-%d-%d %d-%d-%d", temp_buf, &from_year, &from.month, &from.date,
		&to_year, &to.month, &to.date);
    if(from_year<0|| from_year > 2099 || from.month<1 || from.month > 12 || from.date < 1 || from.date > 31
	|| to_year < 0 || to_year > 2099 || to.month < 1 || to.month > 12 || to.date < 1 || to.date > 31)
    {
        tcp_write(pcb23, "invalid parameter\r\n", strlen("invalid parameter\r\n"), TCP_WRITE_FLAG_COPY);
	  return;
    }
    from.year = from_year %2000;
    to.year = to_year %2000;
    //get_sd_data(&from, &to);
}
#endif
void ip_config(char *buf)
{
    int ip[4]={0,0,0,0};
    uint8_t netmask[4]={0,0,0,0};
    uint8_t gw_addr[4]={0,0,0,0};
    char *p = buf;
    uint8_t save_flag=0;
    uint8_t error_flag=0;
    uint8_t index = 0;
#if 1
    p = strstr(p, "ip=");
    if(p)
    { 
        save_flag = 1;
        p = strchr(p, '=');
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      ip[0] = ip[0]*10+ *p-'0';
	      p++;
	  }
	  saveparm.ipaddr[0] = ip[0];
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      ip[1] = ip[1]*10+ *p-'0';
	      p++;
	  }
	  saveparm.ipaddr[1] = ip[1];
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      ip[2] = ip[2]*10+ *p-'0';
	      p++;
	  }
	  saveparm.ipaddr[2] = ip[2];
	  p++;
	  for(index = 0; (*p != ' '); index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      ip[3] = ip[3]*10+ *p-'0';
	      p++;
	  }
	  saveparm.ipaddr[3] = ip[3];
     }

   p = strstr(p, "netmask=");
   if(p)
   {
       save_flag = 1;
       p = strchr(p, '=');
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      netmask[0] = netmask[0]*10+ *p-'0';
	      p++;
	  }
	  saveparm.net_mask[0] = netmask[0];
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      netmask[1] = netmask[1]*10+ *p-'0';
	      p++;
	  }
	  saveparm.net_mask[1] = netmask[1];
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      netmask[2] = netmask[2]*10+ *p-'0';
	      p++;
	  }
	  saveparm.net_mask[2] = netmask[2];
	  p++;
	  for(index = 0; (*p != ' '); index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      netmask[3] = netmask[3]*10+ *p-'0';
	      p++;
	  }
	  saveparm.net_mask[3] = netmask[3];
   }


   p = strstr(p, "gw=");
   if(p)
   {
       save_flag = 1;
       p = strchr(p, '=');
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      gw_addr[0] = gw_addr[0]*10+ *p-'0';
	      p++;
	  }
	  saveparm.gw_addr[0] = gw_addr[0];
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      gw_addr[1] = gw_addr[1]*10+ *p-'0';
	      p++;
	  }
	  saveparm.gw_addr[1] = gw_addr[1];
	  p++;
	  for(index = 0; *p != '.'; index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      gw_addr[2] = gw_addr[2]*10+ *p-'0';
	      p++;
	  }
	  saveparm.gw_addr[2] = gw_addr[2];
	  p++;
	  for(index = 0; (*p != ' '); index++)
	  {
	      if(('\r' == *p) || (index>=3))
			break;
	      if(*p < '0' || *p > '9')
	      	{
	      	   error_flag = 1;
		   goto savep;
	      	}
		
	      gw_addr[3] = gw_addr[3]*10+ *p-'0';
	      p++;
	  }
	  saveparm.gw_addr[3] = gw_addr[3];
   }

 #endif
savep:
  if(save_flag&&(error_flag == 0))
  {
      save_parm();
      showip();
  }
  else if(error_flag)
  	tcp_write(pcb23, "invalid parameter\r\n", strlen("invalid parameter\r\n"), TCP_WRITE_FLAG_COPY);
  else
  	showip();
 }

void showip()
{
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "ip:%d.%d.%d.%d\r\n", saveparm.ipaddr[0], saveparm.ipaddr[1], saveparm.ipaddr[2], saveparm.ipaddr[3]);
    tcp_write(pcb23, buf, strlen(buf), TCP_WRITE_FLAG_COPY);

    memset(buf, 0x00, sizeof(buf));
    snprintf(buf, sizeof(buf), "netmask:%d.%d.%d.%d\r\n", saveparm.net_mask[0], saveparm.net_mask[1], saveparm.net_mask[2], saveparm.net_mask[3]);
    tcp_write(pcb23, buf, strlen(buf), TCP_WRITE_FLAG_COPY);

    memset(buf, 0x00, sizeof(buf));
    snprintf(buf, sizeof(buf), "gateway:%d.%d.%d.%d\r\n", saveparm.gw_addr[0], saveparm.gw_addr[1], saveparm.gw_addr[2], saveparm.gw_addr[3]);
    tcp_write(pcb23, buf, strlen(buf), TCP_WRITE_FLAG_COPY);

    tcp_output(pcb23);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


