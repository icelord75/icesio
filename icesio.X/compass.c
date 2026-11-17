/*
 * File:   compass.c
 * Author: ice
 *
 * Created on April 21, 2019, 7:15 PM
 */


#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "debug.h"

//HMC5883L
#define HMC5883L_ADDRESS     (0x3D / 2) // MCC I2C driver expect 7 bit address. Read and write internal
int16_t X_axis,Y_axis,Z_axis;   // HMC5883L outputs signed 16-bit values

bool HMC5883L_Write(uint8_t reg,uint8_t val)
{
    uint8_t buf[2];
    buf[0]=reg;
    buf[1]=val;
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_MasterWrite( buf, 2, HMC5883L_ADDRESS, &status);
    while (status == I2C1_MESSAGE_PENDING);
    return status == I2C1_MESSAGE_COMPLETE;
}

bool HMC5883L_Init(){
    if (!HMC5883L_Write(0x00,0x70)) return false; //8-average, 15 Hz default or any other rate, normal measurement
    return HMC5883L_Write(0x01,0xA0);             //Gain 5
}

bool HMC5883L_Read()
{
    uint8_t HMC5883L_DATA[6];
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    if (!HMC5883L_Write(0x02,0x01)) return false;  // Single-measurement mode
    I2C1_MasterRead(HMC5883L_DATA, 6, HMC5883L_ADDRESS, &status);
    while (status == I2C1_MESSAGE_PENDING);
    if (status != I2C1_MESSAGE_COMPLETE) return false;
    DBG_PRINTF("DATA: %u %u %u %u %u %u\r\n",HMC5883L_DATA[0],HMC5883L_DATA[1],HMC5883L_DATA[2],HMC5883L_DATA[3],HMC5883L_DATA[4],HMC5883L_DATA[5]);
    // Register order is X, Z, Y (MSB first)
    X_axis=(int16_t)((uint16_t)HMC5883L_DATA[0] << 8 | HMC5883L_DATA[1]);
    Z_axis=(int16_t)((uint16_t)HMC5883L_DATA[2] << 8 | HMC5883L_DATA[3]);
    Y_axis=(int16_t)((uint16_t)HMC5883L_DATA[4] << 8 | HMC5883L_DATA[5]);
    return true;
}
