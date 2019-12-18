#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "utities.h"

#define		MSG_GETINFO 	0x0001
#define 	MSG_COAPSEND 	0x1000

struct AppMsg
{
	int msg_type;
	const char * msg_payload;
};
typedef struct AppMsg *AppMsg_t;

extern struct rt_mailbox mb_coap;
extern struct rt_mailbox mb_control;
extern struct rt_semaphore sem_rs485;

void coap_thread_init(void);
void coap_thread_entry(void *parameter);
void coap_send(AppMsg_t appmsg);

void control_thread_init(void);
void control_thread_entry(void *parameter);
void notify_control(AppMsg_t appmsg);

void soft_timer_init(void);
void rs485_device_init(void);
rt_err_t rs485_get_info(AppMsg_t appmsg);

#endif
