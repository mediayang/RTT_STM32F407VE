#include "bsp_sensor.h"

void sensor_thread_entry(void* parameter)
{
    rt_kprintf("ID is %x\r\n", mpu9250_readData(WHO_AM_I));
    rt_kprintf("gyro_x_l is %x\r\n",mpu9250_readData(GYRO_XOUT_L));
        // for(int i=0;i<len;i++)
        // {
        //     rt_kprintf("buff[%d] is %d\r\n", i, buff[i]);
        // }
        // rt_kprintf("USER_CTRL is %x", temp);
        // rt_kprintf("\n\n");
        // rt_thread_delay(rt_tick_from_millisecond(2000));
       
}