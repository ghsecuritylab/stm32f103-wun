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

#include "protocol.h"

int main(void)
{		
	rs485_device_init();
	
	control_thread_init();
	
	coap_thread_init();
	
	soft_timer_init();
	
    return RT_EOK;
}

