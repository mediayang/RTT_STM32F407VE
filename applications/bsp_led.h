#ifndef BSP_LED_H
#define BSP_LED_H

#include "drv_gpio.h"
#include "kernel/components/drivers/include/drivers/pin.h"

#define LED0 31 //PA6--31
#define LED1 32 //PA7--32

extern void led_thread_entry(void* parameter);

#endif