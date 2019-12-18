#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "protocol.h"

char mb_coap_pool[256];
struct rt_mailbox mb_coap;

void coap_thread_init(void)
{
	rt_err_t result = rt_mb_init(&mb_coap,"Coap MailBox", &mb_coap_pool, sizeof(mb_coap_pool)/4, RT_IPC_FLAG_FIFO);
	
	if(result != RT_NULL)
		rt_kprintf("MailBox Coap create fail\n");
	
	rt_thread_t rtd_coap = rt_thread_create("Coap Thread",coap_thread_entry,  RT_NULL, 1024, 12, 50);
	
	if(rtd_coap != RT_NULL)
		rt_thread_startup(rtd_coap);
}

void coap_thread_entry(void *parameter)
{
	AppMsg_t  appMsg;
	rt_kprintf("Coap Thread start!\n");
	while(1)
	{
		if(rt_mb_recv(&mb_coap, (rt_ubase_t *)&appMsg, RT_WAITING_FOREVER)== RT_EOK)
		{
			switch(appMsg->msg_type)
			{
				case MSG_COAPSEND:
//					rt_sprintf(message,"Coap get message %d\n",appMsg->msg_payload);
//					send_by_uart3(message);
					send_by_uart3(appMsg->msg_payload);
					break;
				
				default:
					rt_kprintf("Coap Invalid Message!\n");
			}
		}
	}
}

void coap_send(AppMsg_t appmsg)
{
	rt_err_t result = rt_mb_send(&mb_coap,(rt_uint32_t)appmsg);
	if(result != RT_NULL)
		rt_kprintf("coap send fail\n");
}

