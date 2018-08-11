#include "drv_spi_mpu9250.h"



static struct rt_spi_device * mpu9250_spi_device = RT_NULL;

rt_uint8_t getDeviceID()
{
    rt_err_t res = 0;
    rt_uint8_t cmd = MPU9250_READ_MARK|WHO_AM_I, recv;
    res = rt_spi_send_then_recv(mpu9250_spi_device, &cmd, 1, &recv, 1);
    RT_ASSERT(res != -RT_EIO);
    return recv; 
}

void mpu9250_write_cmd(const rt_uint8_t reg, rt_uint8_t value)
{
    rt_uint8_t cmd[2] = {reg,value};
    rt_spi_send(mpu9250_spi_device, cmd, 2);
}

rt_int8_t mpu9250_readData(rt_int8_t reg)
{
    rt_int8_t recv;
    reg |=MPU9250_READ_MARK;
    rt_spi_send_then_recv(mpu9250_spi_device, &reg, 1, &recv, 1);
    return recv;
}

void mpu9250_device_initialization()
{
    
    mpu9250_write_cmd(PWR_MGMT_1, 0x80); //Reset mpu9250
    rt_thread_delay(rt_tick_from_millisecond(100));
    //Set clock source
    mpu9250_write_cmd(PWR_MGMT_1, MPU9250_CLOCK_PLLGYROZ);
    rt_thread_delay(rt_tick_from_millisecond(100));

    rt_kprintf("mpu9250's id is 0x%x \r\n", getDeviceID());
    /*MPU9250 Set Sensors*/
    //Enable all xyz
    mpu9250_write_cmd(PWR_MGMT_2, 0x00);
    rt_thread_delay(rt_tick_from_millisecond(10));
    
    mpu9250_write_cmd(CONFIG,0x00);
    rt_thread_delay(rt_tick_from_millisecond(10));
    mpu9250_write_cmd(SMPLRT_DIV, 0x00);//125kHz Sample_Rate = Internal_Sample_Rate/(1 + SMPLRT_DIV)
    rt_thread_delay(rt_tick_from_millisecond(10));
    mpu9250_write_cmd(GYRO_CONFIG,0X18); //2000dps
    rt_thread_delay(rt_tick_from_millisecond(10));
    mpu9250_write_cmd(ACCEL_CONFIG,0x10); //8g
    rt_thread_delay(rt_tick_from_millisecond(10));
    mpu9250_write_cmd(ACCEL_CONFIG_2,0x00); //8g
    rt_thread_delay(rt_tick_from_millisecond(10));
    mpu9250_write_cmd(GYRO_CONFIG,3<<3); // +2000dps
    rt_thread_delay(rt_tick_from_millisecond(10));
    
}

int rt_hw_mpu9250_spi_init(void)
{
    int res = 0;
    //注册设备并挂载到spi的bus总线上
    res = stm32_spi_bus_attach_device(cs_pin,SPI_BUS_NAME,SPI_MPU9250_DEVICE_NAME);
    if(res != RT_EOK)
    {
        rt_kprintf("mpu9250 can't attach the spi bus %s!\r\n", SPI_MPU9250_DEVICE_NAME);
        return res;
    }
    
    mpu9250_spi_device = (struct rt_spi_device *)rt_device_find(SPI_MPU9250_DEVICE_NAME);
    RT_ASSERT(mpu9250_spi_device != RT_NULL);
    //配置通讯参数
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
        cfg.max_hz = 1 * 1000 *1000; /* 1M */

        rt_spi_configure(mpu9250_spi_device, &cfg);
    }

    mpu9250_device_initialization();

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_mpu9250_spi_init);