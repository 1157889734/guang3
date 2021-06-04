#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "telnet.h"

#define UDP_SERVER_PORT 6000
u8_t buf[128];
struct udp_pcb *upcbserv;
struct ip_addr client_addr;
u8_t monitor_flag = 0;
u16_t client_port;
void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
void udp_server_init(void)
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_server_receive_callback, NULL);
      }
      else
      {
        printf("can not bind pcb");
      }
   }
   else
   {
     printf("can not create pcb");
   } 
}


void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  /* Connect to the remote client */
  char monitor[16] = "get_data";
  char release[12] = "release";
  u16_t  index = 0;
  char *data;
  //udp_connect(upcb, addr, port);
    memcpy((void*)&client_addr, (void*)addr, sizeof(struct ip_addr));
    client_port = port;
    
  /* Tell the client that we have accepted it */
  //udp_send(upcb, p);

  /* free the UDP connection, so we can accept new clients */

  upcbserv = upcb;
  data = p->payload;
  while(index < p->len && (p->len - index) > strlen(monitor))
  {
      if(strncmp((char*)(data+index), monitor, strlen(monitor)) == 0)
      	{
      	    monitor_flag = 1;
	    break;
      	}
	index++;
  }

  index = 0;
  while(index < p->len && (p->len - index) > strlen(release))
  {
      if(strncmp((char*)(data+index), release, strlen(release)) == 0)
      	{
      	    monitor_flag = 0;
	    break;
      	}
	index++;
  }
	
  /* Free the p buffer */
  pbuf_free(p);
   
}

//void send_int_buf_by_udp(unsigned char *buf, int length)
void send_int_buf_by_udp(void)
{
      u8_t cnt;
	static u32_t index = 0;
	for(cnt = 0; cnt < 128; cnt++)
		buf[cnt] =( index + cnt)%26 + 'a';
	buf[0] = index%25 + 'A';
	index++;
	
 	
	if (upcbserv == NULL || !monitor_flag) {
	      return;
	    } 
	else {
		struct pbuf *pkt_buf = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
		pkt_buf->payload = (void*)buf;
	      pkt_buf->len =pkt_buf->tot_len = 128;
		//udp_send(upcbserv, pkt_buf);
		udp_sendto(upcbserv, pkt_buf, (struct ip_addr *)&client_addr, client_port);
		pbuf_free(pkt_buf);
	}	 
}
