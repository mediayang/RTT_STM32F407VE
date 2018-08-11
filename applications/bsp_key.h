#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "drv_gpio.h"
#include "kernel/components/drivers/include/drivers/pin.h"

#define KEY0 3
#define KEY1 2

extern void key_thread_entry(void* parameter);
extern void key_irq_thread_entry(void* parameter);

#endif