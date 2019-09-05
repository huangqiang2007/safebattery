/*
 * pwrDetect.c
 *
 *  Created on: 2017Äê7ÔÂ6ÈÕ
 *      Author: Administrator
 */
#include "pwrDetect.h"
#include "em_i2c.h"
#include "em_gpio.h"
#include "main.h"
#include "i2cspm.h"
#include <stddef.h>

#define RSENSE_BATTERY          2.5

#define QHALF_BATTERY           32768
/* 7.35Ah */
#define QMAX_BATTERY            7350
/* QLSB > QMAX / 65535 = 0.1122 */
/* QLSB:  6.8mAh = 0.34mAh * (50m / RSENSE_BATTERY)                        */
/*        6.8(M=4096) 1.7(M=1024) 0.425(M=256) 0.10625(M=64) 0.02656(M=16) */
#define QLSB_BATTERY            0.425
/* percent of battery: 50% + (q - QHALF_BATTERY) * QLSB_BATTERY / QMAX_BATTERY */
/* Q = (PERCENT - 50%) * QMAX_BATTERY / QLSB_BATTERY + QHALF_BATTERY */

/* MODE:      manual    [7:6] = 01
 * M:         256       [5:3] = 100
 * ALCCn:     disabled  [2:1] = 00
 * SHUTDOWN:  disabled  [0]   = 0 */
#define LTC2944_CMD   0x60
#define LTC4151_CMD   0x08
#define ADM1176_CMD   0x15

#define LTC2944_V_FULL_SCALE    70.8f
#define LTC2944_I_FULL_SCALE    64.0f

#define LTC4151_V_FULL_SCALE    102.4f
#define LTC4151_I_FULL_SCALE    81.92f
#define RSENSE_DRIVER           5

#define ADM1176_V_FULL_SCALE    6.65f
#define ADM1176_I_FULL_SCALE    105.84f
#define RSENSE_B11              5
#define RSENSE_B12              5
#define RSENSE_B13              5

static uint32_t BartteryMeasStart()
{
  int32_t ret;
  ret = I2C_reg_write(I2C1, I2C_ADDR_BATTERY_POWER, I2C_REG_LTC2944_CTRL, LTC2944_CMD);
  if (ret != 0)
    return 1;

  return 0;
}

static uint32_t BartteryMeasCheck()
{
  int32_t ret;
  uint8_t data;
  ret = I2C_reg_read(I2C1, I2C_ADDR_BATTERY_POWER, I2C_REG_LTC2944_CTRL, &data);
  if ((ret != 0) || ((data & 0xC0) != 0))
    return 1;

  return 0;
}

uint32_t powerDetectorInit(void)
{
  int32_t ret;
  uint32_t status = 0;

  ret = BartteryMeasStart();
  if (ret != 0)
    status = 1;

  ret = I2C_reg_write(I2C0, I2C_ADDR_B11_POWER, I2C_REG_LTC4151_CTRL, LTC4151_CMD);
  if (ret != 0)
    status = 1;
  ret = I2C_reg_write(I2C0, I2C_ADDR_B12_POWER, I2C_REG_LTC4151_CTRL, LTC4151_CMD);
  if (ret != 0)
    status = 1;
  ret = I2C_reg_write(I2C0, I2C_ADDR_B13_POWER, I2C_REG_LTC4151_CTRL, LTC4151_CMD);
  if (ret != 0)
    status = 1;

  ret = I2C_write_command(I2C0, I2C_ADDR_DRIVER_POWER, ADM1176_CMD);
  if (ret != 0)
    status = 1;

  return status;
}

uint32_t setBarrteryQuantity(uint8_t percent)
{
  int32_t ret;
  uint16_t quantity;

  /* Q = (PERCENT - 50%) * QMAX_BATTERY / QLSB_BATTERY + QHALF_BATTERY */
  quantity = (percent - 50) / 100.0f * QMAX_BATTERY / QLSB_BATTERY + QHALF_BATTERY;

  ret = I2C_reg16_write(I2C1, I2C_ADDR_BATTERY_POWER, I2C_REG_LTC2944_QH, quantity);
  if (ret != 0)
    return 1;

  return 0;
}

uint32_t getBatteryPower_info(uint16_t *voltage, uint16_t *current, uint8_t *quantity)
{
  int32_t ret;
  uint8_t data[2];
  uint16_t result;

  if (BartteryMeasCheck() == 0) {
    ret = I2C_reg_reads(I2C1, I2C_ADDR_BATTERY_POWER, I2C_REG_LTC2944_QH, data, 2);
    if (ret != 0)
      return 1;
    result = (data[0] << 8) | data[1];
    /* percent of battery: 50% + (q - QHALF_BATTERY) * QLSB_BATTERY / QMAX_BATTERY */
    *quantity = 50 + (result - QHALF_BATTERY) * QLSB_BATTERY / QMAX_BATTERY * 100;

    ret = I2C_reg_reads(I2C1, I2C_ADDR_BATTERY_POWER, I2C_REG_LTC2944_VH, data, 2);
    if (ret != 0)
      return 1;
    result = (data[0] << 8) | data[1];
    *voltage = (uint16_t)(LTC2944_V_FULL_SCALE * result / 65535 * 1000);

    ret = I2C_reg_reads(I2C1, I2C_ADDR_BATTERY_POWER, I2C_REG_LTC2944_IH, data, 2);
    if (ret != 0)
      return 1;
    result = (data[0] << 8) | data[1];
    *current = (uint16_t)(LTC2944_I_FULL_SCALE * (result - 32767) / 32767 / RSENSE_BATTERY * 1000);

    BartteryMeasStart();

    return 0;
  }

  return 1;
}

uint32_t getDriverPower_info(uint16_t *voltage, uint16_t *current)
{
  int32_t ret;
  uint8_t data[3];
  uint16_t result;

  ret = I2C_Read(I2C0, I2C_ADDR_DRIVER_POWER, data, 3);
  if (ret != 0)
    return 1;

  result = (data[0] << 4) | (data[2] >> 4);
  *voltage = (uint16_t)(ADM1176_V_FULL_SCALE * result / 4096 * 1000);
  result = (data[1] << 4) | (data[2] & 0x0F);
  *current = (uint16_t)(ADM1176_I_FULL_SCALE * result / 4096 / RSENSE_DRIVER * 1000);

  return 0;
}

uint32_t getB11Power_info(uint16_t *voltage, uint16_t *current)
{
  int32_t ret;
  uint8_t data[4];
  uint16_t result;

  ret = I2C_reg_reads(I2C0, I2C_ADDR_B11_POWER, I2C_REG_LTC4151_IH, data, 4);
  if (ret != 0)
    return 1;
  result = data[0] << 4;
  result |= data[1] >> 4;
  *current = (uint16_t)(LTC4151_I_FULL_SCALE * result / 4096 / RSENSE_B11 * 1000);

  result = data[2] << 4;
  result |= data[3] >> 4;
  *voltage = (uint16_t)(LTC4151_V_FULL_SCALE * result / 4096 * 1000);

  return 0;
}

uint32_t getB12Power_info(uint16_t *current)
{
  int32_t ret;
  uint8_t data[2];
  uint16_t result;

  ret = I2C_reg_reads(I2C0, I2C_ADDR_B12_POWER, I2C_REG_LTC4151_IH, data, 2);
  if (ret != 0)
    return 1;
  result = data[0] << 4;
  result |= data[1] >> 4;
  *current = (uint16_t)(LTC4151_I_FULL_SCALE * result / 4096 / RSENSE_B12 * 1000);

  return 0;
}

uint32_t getB13Power_info(uint16_t *current)
{
  int32_t ret;
  uint8_t data[2];
  uint16_t result;

  ret = I2C_reg_reads(I2C0, I2C_ADDR_B13_POWER, I2C_REG_LTC4151_IH, data, 2);
  if (ret != 0)
    return 1;
  result = data[0] << 4;
  result |= data[1] >> 4;
  *current = (uint16_t)(LTC4151_I_FULL_SCALE * result / 4096 / RSENSE_B13 * 1000);

  return 0;
}
