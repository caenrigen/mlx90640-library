#include "MLX90640_RPI_I2C_Driver.h"
#include <iostream>
#include <bcm2835.h>

int MLX90640_I2CInit()
{
    int success;
    success = bcm2835_init();
    if(!success) return !success;
    bcm2835_i2c_end();
    return !bcm2835_i2c_begin();
}

int MLX90640_I2CEnd()
{
    bcm2835_i2c_end();
    return !bcm2835_close();
}

int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    int result;
    char cmd[2] = {(char)(startAddress >> 8), (char)(startAddress & 0xFF)};

    bcm2835_i2c_setSlaveAddress(slaveAddr);

    char buf[1664];
    uint16_t *p = data;

    result = bcm2835_i2c_write_read_rs(cmd, 2, buf, nMemAddressRead * 2);

    for(int count = 0; count < nMemAddressRead; count++){
        int i = count << 1;
        *p++ = ((uint16_t)buf[i] << 8) | buf[i+1];
    }
    return result;
} 

void MLX90640_I2CFreqSet(uint32_t freq)
{
    bcm2835_i2c_set_baudrate(freq);
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
    char cmd[4] = {(char)(writeAddress >> 8), (char)(writeAddress & 0x00FF), (char)(data >> 8), (char)(data & 0x00FF)};
    return bcm2835_i2c_write(cmd, 4);
}