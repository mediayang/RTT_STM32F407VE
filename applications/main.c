/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <board.h>

#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_sensor.h"

int main(void)
{
  /* user app entry */
  //rt_kprintf("hello");
  rt_thread_t tid;
  tid = rt_thread_create("key",
                          key_irq_thread_entry,
                          RT_NULL,
                          1024,
                          4,
                          10);
  if(tid !=RT_NULL)
    rt_thread_startup(tid);
  
  // tid = rt_thread_create("key1",
  //                        key_thread_entry,
  //                        RT_NULL,
  //                        1024,
  //                        2,
  //                        10);
  // /* 创建成功则启动线程 */
  // if (tid != RT_NULL)
  //   rt_thread_startup(tid);

  /* 创建led线程 */
  tid = rt_thread_create("led",
                         led_thread_entry,
                         RT_NULL,
                         1024,
                         3,
                         10);
  /* 创建成功则启动线程 */
  if (tid != RT_NULL)
    rt_thread_startup(tid);

/* 创建led线程 */
  tid = rt_thread_create("sensor",
                         sensor_thread_entry,
                         RT_NULL,
                         1024,
                         3,
                         10);
  /* 创建成功则启动线程 */
  if (tid != RT_NULL)
    rt_thread_startup(tid);

  return 0;
}





