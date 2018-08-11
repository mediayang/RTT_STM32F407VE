#include "bsp_led.h"

void led_thread_entry(void* parameter)
{    
    //设置管脚为输出模式      
    rt_pin_mode(LED0, PIN_MODE_OUTPUT); 
    //设置管脚为输出模式
    rt_pin_mode(LED1, PIN_MODE_OUTPUT); 

    while (1)
    {   
        //输出低电平，LED0 亮
        rt_pin_write(LED0, PIN_LOW);
        //输出低电平，LED1 亮   
        rt_pin_write(LED1, PIN_HIGH);     
        //挂起 500ms
        rt_thread_delay(rt_tick_from_millisecond(500));
        //输出高电平，LED0 灭
        rt_pin_write(LED0, PIN_HIGH);
        //输出高电平，LED1 灭
        rt_pin_write(LED1, PIN_LOW);
        //挂起 500ms
        rt_thread_delay(rt_tick_from_millisecond(500));       
    }            
}