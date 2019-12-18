#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "protocol.h"

#define MSGQUE_MAX_SIZE 10	
#define MAX_LENGTH_RS485_MSG 32

static struct rt_messagequeue mq_rs485;
static rt_device_t rs485;
struct rt_semaphore sem_rs485;

static 	char 	mq_rs485_pool[MAX_LENGTH_RS485_MSG * MSGQUE_MAX_SIZE];

static 	char	rs485_val[MAX_LENGTH_RS485_MSG];
static  char 	buffer[MAX_LENGTH_RS485_MSG];		
	
static rt_err_t rs485_process(rt_device_t dev, rt_size_t size)
{
	char buf[size];
	
	rt_device_read(rs485, 0, &buf, size);
		
	rt_sprintf(buffer,"RS485 online %s\n",buf);
	rt_kprintf(buffer);
	
	rt_err_t ret = rt_mq_send(&mq_rs485, &buffer, MAX_LENGTH_RS485_MSG);
	if(ret != RT_NULL)
	{
		rt_kprintf("MessageQueue rs485 fail\n");
		return RT_EIO;
	}
    return RT_EOK;
}

static void rs485_thread_entry(void * parameter)
{
	rt_kprintf("RS485 Thread start!\n");
	while(1)
	{
		if(rt_mq_recv(&mq_rs485, rs485_val, sizeof(rs485_val), RT_WAITING_FOREVER)== RT_EOK)
		{
			rt_sem_release(&sem_rs485);
		}
	}
}

void rs485_device_init(void)
{
	rs485 = rt_device_find("uart2");
	
	if(rs485 != RT_NULL)
	{
		rt_err_t result = rt_device_open(rs485, RT_DEVICE_FLAG_INT_RX);
		if(result != RT_NULL)
		{
			rt_kprintf("rs485 open fail\n");
			return;
		}
		
		rt_device_set_rx_indicate(rs485,rs485_process);
		
		result = rt_mq_init(&mq_rs485, "mq_rs485", &mq_rs485_pool[0], MAX_LENGTH_RS485_MSG, sizeof(mq_rs485_pool), RT_IPC_FLAG_FIFO);
		if(result != RT_EOK)
		{
			rt_kprintf("Failed to init message queue: mq_rs485 \n");
			return;
		}
		
		rt_thread_t rtd_rs485 = rt_thread_create("RS485 Thread", rs485_thread_entry, RT_NULL, 1024, 12, 50);
	
		if(rtd_rs485 != RT_NULL)
			rt_thread_startup(rtd_rs485);		
	}
	
	if ( rt_sem_init(&sem_rs485, "sem_rs485", 0, RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("Failed to init semp: pm_data_sem \n");
		return;
	}	
}

rt_err_t rs485_get_info(AppMsg_t appmsg)
{
	rt_err_t result = rt_sem_take(&sem_rs485,400);
	while(sem_rs485.value)
		rt_sem_trytake(&sem_rs485);
	
	if(result == RT_EOK)
	{
		appmsg->msg_payload = rs485_val;
		return RT_EOK;
	}
	else
	{
		return result;
	}
}
