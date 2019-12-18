/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

static char rmt_send_pool[256];
static struct rt_mailbox rmt_send;

struct AppMsg
{
	int msg_type;
};
typedef struct AppMsg AppMsg_t;

void timeout1(void *parameter)
{
	static int tick1=0;
	rt_kprintf("time1 %d\n",tick1++);
}

void timeout2(void *parameter)
{
	static int tick2=0;
	rt_kprintf("time2 %d\n",tick2++);
}

void control_thread_entry(void *parameter)
{
	AppMsg_t * appMsg;
	while(1)
	{
		if(rt_mb_recv(&rmt_send, (rt_ubase_t *)&appMsg, RT_WAITING_FOREVER)== RT_EOK)
		{
			rt_kprintf("msg:%d\n",appMsg->msg_type);
			switch(appMsg->msg_type)
			{
				case 2:
					rt_kprintf("I get message\n");
					break;
				
				default:
					rt_kprintf("invalid Message type!\n");
			}
		}
	}
}

void coap_thread_entry(void *parameter)
{
	while(1)
	{
		rt_kprintf("this is coap thread\n");
		rt_thread_delay(200);
	}
}

int main(void)
{
    rt_timer_t rtt1 = rt_timer_create("Time1", timeout1, RT_NULL, 1000, RT_TIMER_FLAG_PERIODIC);
	rt_timer_t rtt2 = rt_timer_create("Time2", timeout2, RT_NULL, 400, RT_TIMER_FLAG_PERIODIC);
	
	rt_err_t result = rt_mb_init(&rmt_send,"Send Box", &rmt_send_pool, sizeof(rmt_send_pool)/4, RT_IPC_FLAG_FIFO);
	if(result != RT_NULL)
		rt_kprintf("Send Mainbox create fail\n");
	
	rt_thread_t rtd_control = rt_thread_create("Main Thread", control_thread_entry, RT_NULL, 1024, 12, 100);
	rt_thread_t rtd_coap = rt_thread_create("Coap Thread",coap_thread_entry,  RT_NULL, 1024, 12, 50);
	 
	
	//rt_timer_start(rtt1);
	//rt_timer_start(rtt2);
	if(rtd_control != RT_NULL)
		rt_thread_startup(rtd_control);
	//if(rtd_coap != RT_NULL)
		//rt_thread_startup(rtd_coap);
	
    return RT_EOK;
}

void mySend(void)
{
	AppMsg_t *appmsg = (AppMsg_t *)rt_malloc(sizeof(AppMsg_t));
	appmsg->msg_type=2;
	rt_kprintf("appmsg:%d\n",appmsg->msg_type);
	rt_err_t result = rt_mb_send(&rmt_send,(rt_uint32_t)appmsg);
}
MSH_CMD_EXPORT_ALIAS(mySend, sendmsg, send message);
