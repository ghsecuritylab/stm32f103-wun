#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "protocol.h"

char mb_control_pool[256];
struct rt_mailbox mb_control;

void control_thread_init(void)
{
	
	rt_err_t result = rt_mb_init(&mb_control,"Control MailBox", &mb_control_pool, sizeof(mb_control_pool)/4, RT_IPC_FLAG_FIFO);
	
	if(result != RT_NULL)
		rt_kprintf("MailBox Control create fail\n");
	
	rt_thread_t rtd_control = rt_thread_create("Control Thread", control_thread_entry, RT_NULL, 1024, 12, 100);
	
	if(rtd_control != RT_NULL)
		rt_thread_startup(rtd_control);
}

void control_thread_entry(void *parameter)
{
	AppMsg_t appMsg;
	AppMsg_t sendMsg = (struct AppMsg *)rt_malloc(sizeof(struct AppMsg));;
	rt_err_t result;
	rt_kprintf("Control Thread start!\n");
	while(1)
	{
		if(rt_mb_recv(&mb_control, (rt_ubase_t *)&appMsg, RT_WAITING_FOREVER)== RT_EOK)
		{
			switch(appMsg->msg_type)
			{
				case MSG_GETINFO:
					result = rs485_get_info(sendMsg);
					if(result == RT_EOK)
					{
						rt_kprintf("Device have response\n");
						
					}else{
						rt_kprintf("Device no response\n");
						sendMsg->msg_payload = "RS485 offline\n";
					}
					sendMsg->msg_type = MSG_COAPSEND;
					coap_send(sendMsg);
					break;
				
				default:
					rt_kprintf("Control Invalid Message!\n");
			}
		}
	}
}

void notify_control(AppMsg_t appmsg)
{
	rt_err_t result = rt_mb_send(&mb_control,(rt_uint32_t)appmsg);
	if(result != RT_NULL)
		rt_kprintf("notify fail\n");
}
