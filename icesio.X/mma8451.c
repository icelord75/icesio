
/* Yangosoft  */
/* https://github.com/yangosoft/pic_mma8451 */

#include "mma8451.h"
#include "debug.h"

//Include your MCC generated files for i2c
#include "mcc_generated_files/mcc.h"

uint8_t readRegister8(uint8_t registerAddr) {



    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_MasterWrite(&registerAddr, 1, MMA8451_DEFAULT_ADDRESS, &status);
    while (status == I2C1_MESSAGE_PENDING);

    if (status == I2C1_MESSAGE_COMPLETE) {
        //OK
    }



    uint8_t data;
    status = I2C1_MESSAGE_PENDING;


    I2C1_MasterRead(&data, 1, MMA8451_DEFAULT_ADDRESS, &status);

    while (status == I2C1_MESSAGE_PENDING);

    if (status == I2C1_MESSAGE_COMPLETE) {
        //OK
    }

    return data;
}


void writeRegister8(uint8_t addr, uint8_t value) {
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    uint8_t data[2];
    data[0] = addr;
    data[1] = value;

    I2C1_MasterWrite(data, 2, MMA8451_DEFAULT_ADDRESS, &status);
    while (status == I2C1_MESSAGE_PENDING);

    if (status == I2C1_MESSAGE_COMPLETE) {
       //OK
    }
}


bool MMAInit(uint8_t wakeupTreshold) {
    
    uint16_t timeout = 0x00FF;
    if (true == I2C1_MasterQueueIsFull() )
    {
        return false;
    }
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    uint8_t writeBuffer[3];
    writeBuffer[0] = 0x0D;
    I2C1_MasterWriteTRBBuild(&readTRB[0],
            writeBuffer,
            1,
            0x1C);

    uint8_t pData;
    I2C1_MasterReadTRBBuild(&readTRB[1],
            &pData,
            1,
            0x1C);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(2, readTRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            DBG_PRINTF(".");
            timeout--;
            if(0 == timeout)
            {
                return false;
            }
        };

        if (status == I2C1_MESSAGE_COMPLETE) {
            DBG_PRINTF("\nTrans complete! %d \n", pData);
            if (pData != 0x1a) {
                return false;
            }
            break;
        }
    }

    if (status != I2C1_MESSAGE_COMPLETE)
    {
        return false;
    }
    
    DBG_PRINTF("Reset\n");

    writeRegister8(MMA8451_REG_CTRL_REG2, 0x40); // reset

    while (readRegister8(MMA8451_REG_CTRL_REG2) & 0x40);

    // enable 4G range
    writeRegister8(MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_2_G);

    // auto sleep high resolution 100Hz
    writeRegister8(MMA8451_REG_CTRL_REG2, 0x4 | 0x02);


    writeRegister8(MMA8451_REG_MOTION_CFG, 0x58); //ELE=0, OAE=1, Z=0, YX=1
    //writeRegister8(MMA8451_REG_THRESHOLD, 0x80| 0x30); //Threshold a 3g. 3g/0.063g/contador = 48 ; DDCNTM=1
    writeRegister8(MMA8451_REG_THRESHOLD, 0x80 | wakeupTreshold ); //Threshold a 3g. 3g/0.063g/contador = 48 ; DDCNTM=1

    //motion wake up
    writeRegister8(MMA8451_REG_CTRL_REG3, 0x08);

    //Setup time sleep
    writeRegister8(MMA8451_REG_ASLP_COUNT, 10); //320ms * 10 a 100Hz

    // motion interrupt
    writeRegister8(MMA8451_REG_CTRL_REG4, 0x04);
    writeRegister8(MMA8451_REG_CTRL_REG5, 0x00); //1 a int1 y 0 a int2

    // Turn on orientation config
    writeRegister8(MMA8451_REG_PL_CFG, 0x40);

    // Activate at max rate, low noise mode
    //writeRegister8(MMA8451_REG_CTRL_REG1, 0xC4 | 0x01);
    writeRegister8(MMA8451_REG_CTRL_REG1, 0xC4 | 0x01);


    return true;
}
uint8_t readRegister2(uint8_t address) {
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    uint8_t writeBuffer[3];
    writeBuffer[0] = address;
    I2C1_MasterWriteTRBBuild(&readTRB[0],
            writeBuffer,
            1,
            0x1C);

    uint8_t pData;
    I2C1_MasterReadTRBBuild(&readTRB[1],
            &pData,
            1,
            0x1C);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(2, readTRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            DBG_PRINTF(".");
        };

        if (status == I2C1_MESSAGE_COMPLETE) {
            DBG_PRINTF("\nTrans complete! %d \n", pData);
            return pData;

        }
    }
    return 0xFF;
}

void MMAGetAccel(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t data[6];
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    uint8_t regOut = MMA8451_REG_OUT_X_MSB;



    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    uint8_t writeBuffer[3];
    writeBuffer[0] = MMA8451_REG_OUT_X_MSB;
    I2C1_MasterWriteTRBBuild(&readTRB[0],
            writeBuffer,
            1,
            0x1C);


    I2C1_MasterReadTRBBuild(&readTRB[1],
            data,
            6,
            0x1C);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(2, readTRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            DBG_PRINTF(".");
        };

        if (status == I2C1_MESSAGE_COMPLETE) {
            DBG_PRINTF("\nTrans complete!\n");
            //OK
    

            *x = data[0];
            *x <<= 8;
            *x |= data[1];
            *x >>= 2;
            *y = data[2];
            *y <<= 8;
            *y |= data[3];
            *y >>= 2;
            *z = data[4];
            *z <<= 8;
            *z |= data[5];
            *z >>= 2;

            break;
        }
    }


}
