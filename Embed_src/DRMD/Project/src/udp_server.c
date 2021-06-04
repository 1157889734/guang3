#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "telnet.h"

#define UDP_SERVER_PORT 6000
struct udp_pcb *upcbserv;
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
  udp_connect(upcb, addr, port);
    
  /* Tell the client that we have accepted it */
  //udp_send(upcb, p);

  /* free the UDP connection, so we can accept new clients */
 // udp_disconnect(upcb);
  upcbserv = upcb;
  parse_input_parm(p->payload, p->len);
	
  /* Free the p buffer */
  pbuf_free(p);
   
}

void send_int_buf_by_udp(unsigned char *buf, int length)
{
 	struct pbuf *pkt_buf = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
	if (upcbserv == NULL) {
	      return;
	    } 
	else {
		pkt_buf->payload = (void*)buf;
	      pkt_buf->len =pkt_buf->tot_len = length;
		udp_send(upcbserv, pkt_buf);
		//udp_sendto(upcbserv, pkt_buf, ipaddr_serv, 10000);
	}

	 pbuf_free(pkt_buf);
}
