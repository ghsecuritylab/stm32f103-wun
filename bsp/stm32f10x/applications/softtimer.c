#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "protocol.h"

static AppMsg_t notifyMsg;

void timeout1(void *parameter)
{
	notifyMsg->msg_type = MSG_GETINFO;
	notify_control(notifyMsg);
}

void soft_timer_init(void)
{
	notifyMsg = (struct AppMsg *)rt_malloc(sizeof(struct AppMsg));
	
    rt_timer_t rtt1 = rt_timer_create("Time1", timeout1, RT_NULL, 500, RT_TIMER_FLAG_PERIODIC);
	
	rt_timer_start(rtt1);
}
