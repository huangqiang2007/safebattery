/*
 * pwrDetect.h
 *
 *  Created on: 2017Äê7ÔÂ6ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_PWRDETECT_H_
#define INCLUDE_PWRDETECT_H_

#include <stdint.h>

#define LTC2944_CMD   0x60
#define LTC4151_CMD   0x08
#define ADM1176_CMD   0x15

/* Battery power detector address */
#define I2C_ADDR_BATTERY_POWER	0xC8

#define I2C_REG_LTC2944_CTRL    0x01
#define I2C_REG_LTC2944_QH	    0x02
#define I2C_REG_LTC2944_QL	    0x03
#define I2C_REG_LTC2944_VH	    0x08
#define I2C_REG_LTC2944_VL	    0x09
#define I2C_REG_LTC2944_IH      0x0E
#define I2C_REG_LTC2944_IL      0x0F

/* B11 power detector address */
#define I2C_ADDR_B11_POWER      0xDE
#define I2C_REG_LTC4151_IH      0x00
#define I2C_REG_LTC4151_IL      0x01
#define I2C_REG_LTC4151_VH      0x02
#define I2C_REG_LTC4151_VL      0x03
#define I2C_REG_LTC4151_CTRL    0x06
/* B12 power detector address */
#define I2C_ADDR_B12_POWER      0xCE

/* B13 power detector address */
#define I2C_ADDR_B13_POWER      0xD2

/* Driver module power detector address */
#define I2C_ADDR_DRIVER_POWER		0x86


uint32_t powerDetectorInit(void);
uint32_t setBarrteryQuantity(uint8_t percent);
uint32_t getBatteryPower_info(uint16_t *voltage, uint16_t *current, uint8_t *quantity);
uint32_t getB11Power_info(uint16_t *voltage, uint16_t *current);
uint32_t getB12Power_info(uint16_t *current);
uint32_t getB13Power_info(uint16_t *current);
uint32_t getDriverPower_info(uint16_t *voltage, uint16_t *current);

#endif /* INCLUDE_PWRDETECT_H_ */
