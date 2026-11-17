/*
 * File:   bme280.c
 * Author: ice
 *
 * Created on May 4, 2019, 7:59 PM
 */

#include "mcc_generated_files/mcc.h"
#include "xc.h"
#include <string.h>
#include "bme280.h"
#include "debug.h"

#define ADR_I2C_BME280 0x76

I2C1_MESSAGE_STATUS BMERead(uint8_t adr, uint16_t cnt, uint8_t *buffer) {
    I2C1_MESSAGE_STATUS volatile i2cStatus = I2C1_MESSAGE_PENDING;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    I2C1_MasterWriteTRBBuild(&readTRB[0], &adr, 1, ADR_I2C_BME280);
    I2C1_MasterReadTRBBuild(&readTRB[1], buffer, cnt, ADR_I2C_BME280);
    I2C1_MasterTRBInsert(2, readTRB, (I2C1_MESSAGE_STATUS *)&i2cStatus);
    // Wait while pending only: a NACK/fail must not hang here forever
    while(i2cStatus == I2C1_MESSAGE_PENDING);
    return i2cStatus;
}

static uint8_t writeBuffer[10];

I2C1_MESSAGE_STATUS BMEWrite(uint8_t adr, uint16_t cnt, uint8_t *buffer) {
    I2C1_MESSAGE_STATUS volatile i2cStatus = I2C1_MESSAGE_FAIL;
    if (cnt < sizeof(writeBuffer) - 1) {
        // first 1 byte of buffer is register address
        writeBuffer[0] = adr;
        memcpy(writeBuffer + 1, buffer, cnt);
        i2cStatus = I2C1_MESSAGE_PENDING;
        // Send register address + data from writeBuffer (was &adr: sent stack garbage)
        I2C1_MasterWrite(writeBuffer, cnt + 1, ADR_I2C_BME280, (I2C1_MESSAGE_STATUS *)&i2cStatus);
        while(i2cStatus == I2C1_MESSAGE_PENDING);
    }
    return i2cStatus;       
}

typedef uint32_t BME280_U32_t;
typedef int32_t BME280_S32_t;typedef int64_t BME280_S64_t;

#define ID_ADR          0xD0
#define RESET_ADR       0xE0
#define RESET_VAL       0xB6
#define CTRL_HUM_ADR    0xF2
#define CTRL_HUM_VAL    0x01 //  1x samples of humidity
#define STATUS_ADR      0xF3
#define CTRL_MEAS_ADR   0xF4
#define CTRL_MEAS_VAL   0b00100101// 1x samples of temp and pressure in force mode
#define CONFIG_ADR      0xF5
#define CONFIG_VAL      0x00    // no filter and forced trigger //0b10101100 // 1Hz, 8x filter Coeff

#define DATA_BLK_ADR    0xF7
#define DATA_BLK_LEN    8

uint8_t dataRaw[DATA_BLK_LEN];

uint32_t    pressure;
int32_t    temperature;
uint32_t    humidity;

//starts at 0x88
#define COMP_BLK1_ADR 0x88
#define COMP_BLK1_LEN sizeof(Blk1_t)
typedef struct {
    uint16_t    dig_T1;
    int16_t     dig_T2;
    int16_t     dig_T3;
    uint16_t    dig_P1;
    int16_t     dig_P2;
    int16_t     dig_P3;
    int16_t     dig_P4;
    int16_t     dig_P5;
    int16_t     dig_P6;
    int16_t     dig_P7;
    int16_t     dig_P8;
    int16_t     dig_P9;
    uint8_t     unused; //0xA0
    uint8_t     dig_H1;
}Blk1_t;

// starts at 0xE1
#define COMP_BLK2_ADR 0xE1
#define COMP_BLK2_LEN sizeof(Blk2_t)
typedef struct {
    int16_t     dig_H2;
    uint8_t     dig_H3;
}Blk2_t;

// starts at 0xE4
#define COMP_BLK3_ADR 0xE4


typedef struct {
    int16_t     dig_H4;
    int16_t     dig_H5;
    uint8_t     dig_H6;
}Blk3_t;

static Blk1_t blk1;
static Blk2_t blk2;
static Blk3_t blk3;

void BME280_Init(void) {
    
#ifdef DEBUG
    uint8_t bme_id;
    BMERead(ID_ADR, 1, &bme_id);
    DBG_PRINTF("BME: %02x\r\n", bme_id);
#endif

    uint8_t temp = RESET_VAL;
    BMEWrite(RESET_ADR, 1, &temp);
    __delay_us(2000);
    
    BMERead(COMP_BLK1_ADR, COMP_BLK1_LEN, (uint8_t *)&blk1);
    BMERead(COMP_BLK2_ADR, COMP_BLK2_LEN, (uint8_t *)&blk2);

    uint8_t rawData[4];
    BMERead(COMP_BLK3_ADR, sizeof(rawData), rawData);
    // now fix up blk3, someone went too far to save a byte
    blk3.dig_H4 = ((uint16_t)rawData[0] << 4) + (rawData[1] & 0x0F); 
    blk3.dig_H5 = ((uint16_t)rawData[2] << 4) + (rawData[1] >> 4); 
    blk3.dig_H6 = rawData[3];
    
    temp = CONFIG_VAL;
    BMEWrite(CONFIG_ADR, 1, &temp);
    temp = CTRL_HUM_VAL;
    BMEWrite(CTRL_HUM_ADR, 1, &temp);
    temp = CTRL_MEAS_VAL;
    BMEWrite(CTRL_MEAS_ADR, 1, &temp);
}

BME280_S32_t t_fine;


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of ?5123? equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T) {
    BME280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BME280_S32_t) blk1.dig_T1 << 1))) * ((BME280_S32_t) blk1.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t) blk1.dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t) blk1.dig_T1))) >> 12) *
            ((BME280_S32_t) blk1.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of ?24674867? represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P) {
    BME280_S64_t var1, var2, p;
    var1 = (BME280_S64_t)t_fine - (BME280_S64_t)128000;
    var2 = var1 * var1 * (BME280_S64_t) blk1.dig_P6;
    var2 = var2 + ((var1 * (BME280_S64_t) blk1.dig_P5) << 17);
    var2 = var2 + (((BME280_S64_t) blk1.dig_P4) << 35);
    var1 = ((var1 * var1 * (BME280_S64_t) blk1.dig_P3) >> 8) + ((var1 * (BME280_S64_t) blk1.dig_P2) << 12);
    var1 = (((((BME280_S64_t) 1) << 47) + var1))*((BME280_S64_t) blk1.dig_P1) >> 33;
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2)*3125) / var1;
    var1 = (((BME280_S64_t) blk1.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BME280_S64_t) blk1.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BME280_S64_t) blk1.dig_P7) << 4);
    return (BME280_U32_t) p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of ?47445? represents 47445/1024 = 46.333 %RH
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H) {
    BME280_S32_t v_x1_u32r;
    v_x1_u32r = t_fine - (BME280_S32_t)76800;
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t) blk3.dig_H4) << 20) - (((BME280_S32_t) blk3.dig_H5) * v_x1_u32r)) +
            ((BME280_S32_t) 16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t) blk3.dig_H6)) >> 10) * (((v_x1_u32r *
            ((BME280_S32_t) blk2.dig_H3)) >> 11) + ((BME280_S32_t) 32768))) >> 10) + ((BME280_S32_t) 2097152)) *
            ((BME280_S32_t) blk2.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t) blk1.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > (BME280_S32_t)419430400 ? (BME280_S32_t)419430400 : v_x1_u32r);
    return (BME280_U32_t) (v_x1_u32r >> 12);
}

void BME280_UpdateValues(void) {
//    uint8_t     dataRaw[DATA_BLK_LEN];
    uint8_t     temp;
    uint16_t    cnt;
    uint8_t     status;
    uint32_t    pressureRaw;
    uint32_t    temperatureRaw;
    uint32_t    humidityRaw;

    cnt = 0;
    temp = CTRL_HUM_VAL;
    BMEWrite(CTRL_HUM_ADR, 1, &temp);
    temp = CTRL_MEAS_VAL;
    BMEWrite(CTRL_MEAS_ADR, 1, &temp);

//    __delay_us(9000);

    I2C1_MESSAGE_STATUS volatile i2cStatus;
    
    do {
  //      __delay_us(1000);
        if ((i2cStatus = BMERead(STATUS_ADR, 1, &status)) != I2C1_MESSAGE_COMPLETE) break;
        ++cnt;
    } while((status & 0x09) != 0 && cnt < 20);
    
    // Read data only if the bus is OK and the measurement has finished
    if (i2cStatus == I2C1_MESSAGE_COMPLETE && (status & 0x09) == 0) {
        BMERead(DATA_BLK_ADR, DATA_BLK_LEN, dataRaw);
 
        pressureRaw = ((uint32_t)dataRaw[0] << 12) +  ((uint32_t)dataRaw[1] << 4) + ((uint32_t)dataRaw[2] >> 4);
        temperatureRaw = ((uint32_t)dataRaw[3] << 12) +  ((uint32_t)dataRaw[4] << 4) + ((uint32_t)dataRaw[5] >> 4);
        humidityRaw = ((uint32_t)dataRaw[6] << 8) + dataRaw[7];
       
        temperature = BME280_compensate_T_int32(temperatureRaw);
        pressure = BME280_compensate_P_int64(pressureRaw);
        humidity = bme280_compensate_H_int32(humidityRaw);        
        DBG_PRINTF("P:%lu T:%ld H:%lu\r\n", pressure, temperature, humidity);
    }
}

int32_t BME280_Temperature(void) {return temperature;}
uint32_t BME280_Pressure(void) {return pressure; /* /25600.0 */ }
uint32_t BME280_Humidity(void) {return humidity; /* /1024 */ }
