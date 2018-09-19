#include "drv_spi_mpu9250.h"
#include <finsh.h>



static struct rt_spi_device * mpu9250_spi_device = RT_NULL;

void getDeviceID()
{
    rt_err_t res = 0;
    rt_uint8_t cmd = MPU9250_READ_MARK|WHO_AM_I, recv;
    res = rt_spi_send_then_recv(mpu9250_spi_device, &cmd, 1, &recv, 1);

    if(res!=RT_EOK)
    {
        rt_kprintf("error code is %d\r\n",res);
    }else
    {
        rt_kprintf("MPU9250 ID is 0x%x\r\n", recv);
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(getDeviceID, __cmd_mpu, say hello to rtt)

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

rt_err_t mpu9250_readBff(rt_int8_t start_add, rt_int8_t* rcvbff, rt_base_t len)
{
    rt_err_t res;
    start_add |= MPU9250_READ_MARK;
    res = rt_spi_send_then_recv(mpu9250_spi_device, &start_add, 1, rcvbff, len);
    return res;
}

/***************************************************************/
// MPU内部i2c 写入
//I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
//I2C_SLVx_REG:   reg
//I2C_SLVx_Data out:  value
/***************************************************************/
static void i2c_Mag_write(rt_uint8_t reg,rt_uint8_t value)
{
	mpu9250_write_cmd(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR);//设置磁力计地址,mode: write
	mpu9250_write_cmd(I2C_SLV0_REG ,reg);//set reg addr
	mpu9250_write_cmd(I2C_SLV0_DO ,value);//send value	
	rt_thread_delay(rt_tick_from_millisecond(1));//此处因为MPU内部I2C读取速度较慢，延时等待内部写完毕
}
/***************************************************************/
// MPU内部i2c 读取
//I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
//I2C_SLVx_REG:   reg
//return value:   EXT_SENS_DATA_00 register value
/***************************************************************/
static rt_uint8_t i2c_Mag_read(rt_uint8_t reg)
{
	mpu9250_write_cmd(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR|0x80); //设置磁力计地址，mode：read
	mpu9250_write_cmd(I2C_SLV0_REG ,reg);// set reg addr
	//mpu9250_write_cmd(I2C_SLV0_DO ,0xff);//read
	rt_thread_delay(rt_tick_from_millisecond(10));//此处因为MPU内部I2C读取速度较慢，必须延时等待内部读取完毕
	return mpu9250_readData(EXT_SENS_DATA_00);
}

void getMagID()
{
    rt_int8_t res;
    mpu9250_write_cmd(I2C_SLV0_ADDR ,0x0C);
    mpu9250_write_cmd(I2C_SLV0_REG ,0x80);
    rt_thread_delay(rt_tick_from_millisecond(10));
    res = mpu9250_readData(EXT_SENS_DATA_00);
    rt_kprintf("magic device's id is 0x%x\r\n", res);
}
FINSH_FUNCTION_EXPORT_ALIAS(getMagID, __cmd_mag, say hello to rtt)

static rt_err_t mpu9250_get_accel_gryo(rt_int16_t *accel, rt_int16_t *temp, rt_int16_t *gyro)
{
    rt_err_t res;
    rt_base_t len=14;
    uint8_t buf[len];
    res = mpu9250_readBff(ACCEL_XOUT_H,buf,len);
    if(res != RT_EOK)
    {
        rt_kprintf("Reading data is error!");
        return res;
    }
    accel[0] = ((int16_t)buf[0]<<8) + buf[1];
    accel[1] = ((int16_t)buf[2]<<8) + buf[3];
    accel[2] = ((int16_t)buf[4]<<8) + buf[5];
    *temp    = ((int16_t)buf[6]<<8) + buf[7];
    gyro[0]  = ((int16_t)buf[8]<<8) + buf[9];
    gyro[1]  = ((int16_t)buf[10]<<8) + buf[11];
    gyro[2]  = ((int16_t)buf[12]<<8) + buf[13];

    rt_kprintf("acc0=%d, acc1=%d, acc2=%d\r\n",accel[0],accel[1],accel[2]);
    rt_kprintf("gyro0=%d, gyro1=%d, gyro2=%d\r\n",gyro[0],gyro[1],gyro[2]);
    return res;
}
FINSH_FUNCTION_EXPORT_ALIAS(mpu9250_get_accel_gryo, __cmd_mpuacc, read mpu9250 regs)

void mpu9250_device_initialization()
{
    mpu9250_write_cmd(PWR_MGMT_1, 0x80); //Reset mpu9250
    rt_thread_delay(rt_tick_from_millisecond(100));
    getDeviceID();
    //Set clock source
    mpu9250_write_cmd(PWR_MGMT_1, MPU9250_CLOCK_PLLGYROZ);
    rt_thread_delay(rt_tick_from_millisecond(100));

    
/**********************Init SLV0 i2c**********************************/	
//Use SPI-bus read slave0
	mpu9250_write_cmd(INT_PIN_CFG ,0x30);// INT Pin / Bypass Enable Configuration  
	mpu9250_write_cmd(I2C_MST_CTRL,0x4d);//I2C MAster mode and Speed 400 kHz
	mpu9250_write_cmd(USER_CTRL ,0x20); // I2C_MST _EN 
	mpu9250_write_cmd(I2C_MST_DELAY_CTRL ,0x01);//延时使能I2C_SLV0 _DLY_ enable 	
	mpu9250_write_cmd(I2C_SLV0_CTRL ,0x81); //enable IIC	and EXT_SENS_DATA==1 Byte
    /*MPU9250 Set Sensors*/
    //Enable all xyz
    mpu9250_write_cmd(PWR_MGMT_2, 0x00);
    mpu9250_write_cmd(CONFIG,0x00);
    mpu9250_write_cmd(SMPLRT_DIV, 0x00);//125kHz Sample_Rate = Internal_Sample_Rate/(1 + SMPLRT_DIV)
    mpu9250_write_cmd(GYRO_CONFIG,0X18); //2000dps
    mpu9250_write_cmd(ACCEL_CONFIG,0x10); //8g
    mpu9250_write_cmd(ACCEL_CONFIG_2,0x00); //8g
    mpu9250_write_cmd(GYRO_CONFIG,3<<3); // +2000dps
    /**********************Init MAG **********************************/
	i2c_Mag_write(AK8963_CNTL2_REG,0x01); // Reset AK8963
    rt_thread_delay(rt_tick_from_millisecond(100));
	i2c_Mag_write(AK8963_CNTL1_REG,0x12); // use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output
    rt_thread_delay(rt_tick_from_millisecond(20));
}

int rt_hw_mpu9250_spi_init(void)
{
    int res = 0;
    //注册设备并挂载到spi的bus总线上
    res = stm32_spi_bus_attach_device(cs_pin,SPI_BUS_NAME,SPI_MPU9250_DEVICE_NAME);
    if(res != RT_EOK)
    {
        rt_kprintf("mpu9250 can't attach the spi bus %s!\r\n", SPI_BUS_NAME);
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