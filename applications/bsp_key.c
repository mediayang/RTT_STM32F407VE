#include "bsp_key.h"
void key_thread_entry(void* parameter)
{
    rt_pin_mode(KEY0, PIN_MODE_INPUT);
    while(1)
    {
        if(rt_pin_read(KEY0) == PIN_LOW)
        {
            rt_kprintf("hello! This is key0\n");
        }
        //挂起10ms
        rt_thread_delay(rt_tick_from_millisecond(10));  
    }
    
            
}

//按键中断回调函数
void key_exti_callback(void *args)
{
    char *a = args;
    rt_kprintf("key1 is pressed! %s\n", a);
    
}
void key_irq_thread_entry(void* parameter)
{
    rt_pin_mode(KEY1, PIN_MODE_INPUT);

    rt_pin_attach_irq(KEY1, PIN_IRQ_MODE_FALLING, key_exti_callback, (void*)"callback args");

    rt_pin_irq_enable(KEY1, PIN_IRQ_ENABLE);  

}