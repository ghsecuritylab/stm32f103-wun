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

#define		MSG_GETINFO 	0x0000
#define 	MSG_COAPSEND 	0x1000

	
static char mb_control_pool[256];
static struct rt_mailbox mb_control;
static char mb_coap_pool[256];
static struct rt_mailbox mb_coap;

struct AppMsg
{
	int msg_type;
	int msg_payload;
};
typedef struct AppMsg AppMsg_t;

static AppMsg_t * notifyMsg;

void notify(AppMsg_t *appmsg);
void coap_send(AppMsg_t *appmsg);
void send_by_uart2(const char * str);

void timeout1(void *parameter)
{
	notifyMsg->msg_type = MSG_GETINFO;
	notify(notifyMsg);
}

void control_thread_entry(void *parameter)
{
	AppMsg_t * appMsg;
	AppMsg_t * sendMsg;
	static int tick;
	while(1)
	{
		if(rt_mb_recv(&mb_control, (rt_ubase_t *)&appMsg, RT_WAITING_FOREVER)== RT_EOK)
		{
			switch(appMsg->msg_type)
			{
				case MSG_GETINFO:
					sendMsg = (AppMsg_t *)rt_malloc(sizeof(AppMsg_t));
					rt_kprintf("Control get message %d\n",tick);
				
					sendMsg->msg_type = MSG_COAPSEND;
					sendMsg->msg_payload = tick++;
					coap_send(sendMsg);
					break;
				
				default:
					rt_kprintf("Control Invalid Message!\n");
			}
		}
	}
}

void coap_thread_entry(void *parameter)
{
	AppMsg_t * appMsg;
	char message[50];
	while(1)
	{
		if(rt_mb_recv(&mb_coap, (rt_ubase_t *)&appMsg, RT_WAITING_FOREVER)== RT_EOK)
		{
			switch(appMsg->msg_type)
			{
				case MSG_COAPSEND:
					rt_sprintf(message,"Coap get message %d\n",appMsg->msg_payload);
					send_by_uart2(message);
					break;
				
				default:
					rt_kprintf("Coap Invalid Message!\n");
			}
		}
	}
}

int main(void)
{
	rt_err_t result;
	
	notifyMsg = (AppMsg_t *)rt_malloc(sizeof(AppMsg_t));
	
	/*¥¥‘Ï Timer, Thread, MaiBox*/
	
    rt_timer_t rtt1 = rt_timer_create("Time1", timeout1, RT_NULL, 200, RT_TIMER_FLAG_PERIODIC);
	
	result = rt_mb_init(&mb_control,"MailBox Control", &mb_control_pool, sizeof(mb_control_pool)/4, RT_IPC_FLAG_FIFO);
	if(result != RT_NULL)
		rt_kprintf("MailBox Control create fail\n");
	
	result = rt_mb_init(&mb_coap,"MailBox Coap", &mb_coap_pool, sizeof(mb_coap_pool)/4, RT_IPC_FLAG_FIFO);
	if(result != RT_NULL)
		rt_kprintf("MailBox Coap create fail\n");
	
	
	rt_thread_t rtd_control = rt_thread_create("Main Thread", control_thread_entry, RT_NULL, 1024, 12, 100);
	
	rt_thread_t rtd_coap = rt_thread_create("Coap Thread",coap_thread_entry,  RT_NULL, 1024, 12, 50);
	 
	/*∆Ù∂Ø*/
	
	rt_timer_start(rtt1);
	
	if(rtd_control != RT_NULL)
		rt_thread_startup(rtd_control);
	
	if(rtd_coap != RT_NULL)
		rt_thread_startup(rtd_coap);
	
    return RT_EOK;
}

void notify(AppMsg_t *appmsg)
{
	rt_err_t result = rt_mb_send(&mb_control,(rt_uint32_t)appmsg);
	if(result != RT_NULL)
		rt_kprintf("notify fail\n");
}

void coap_send(AppMsg_t *appmsg)
{
	rt_err_t result = rt_mb_send(&mb_coap,(rt_uint32_t)appmsg);
	if(result != RT_NULL)
		rt_kprintf("coap send fail\n");
}

void send_by_uart2(const char * str)
{
	const char * tmp = str;
	char amount=0;
	
	while(*(tmp++) != '\n' && amount<50 )
		amount++;
	
	rt_device_t uart2 = rt_device_find("uart2");
	if(uart2 != RT_NULL)
	{
		rt_err_t result = rt_device_open(uart2,RT_DEVICE_FLAG_INT_TX);
		if(result != RT_NULL)
		{
			rt_kprintf("uart2 open fail\n");
			return;
		}
		rt_device_write(uart2,0,str,amount+1);
	}
}
