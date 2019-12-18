#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "protocol.h"


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
		rt_device_close(uart2);
	}
}

void send_by_uart3(const char * str)
{
	const char * tmp = str;
	char amount=0;
	
	while(*(tmp++) != '\n' && amount<50 )
		amount++;
	
	rt_device_t uart3 = rt_device_find("uart3");
	if(uart3 != RT_NULL)
	{
		rt_err_t result = rt_device_open(uart3,RT_DEVICE_FLAG_INT_TX);
		if(result != RT_NULL)
		{
			rt_kprintf("uart3 open fail\n");
			return;
		}
		rt_device_write(uart3,0,str,amount+1);
		rt_device_close(uart3);
	}
}
