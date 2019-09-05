/*
 * i2cspmconfig.h
 *
 *  Created on: 2017Äê7ÔÂ4ÈÕ
 *      Author: Administrator
 */

#ifndef INLCUDE_I2CSPMCONFIG_H_
#define INLCUDE_I2CSPMCONFIG_H_

#include "em_i2c.h"

/***************************************************************************//**
 * @addtogroup I2CSPM
 * @{
 ******************************************************************************/

/* I2C SPM driver config. This default override only works if one I2C interface
   is in use. If multiple interfaces are in use, define the peripheral setup
   inside the application in a I2CSPM_Init_TypeDef and then pass the initialization
   struct to I2CSPM_Init(). */
#define I2C0SPM_INIT_DEFAULT                                                    \
  { I2C0,                       /* Use I2C instance 0 */                       \
    gpioPortD,                  /* SCL port */                                 \
    7,                          /* SCL pin */                                  \
    gpioPortD,                  /* SDA port */                                 \
    6,                          /* SDA pin */                                  \
    1,                          /* Location */                                 \
    0,                          /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,      /* Set to standard rate  */                    \
    i2cClockHLRStandard,        /* Set to use 4:4 low/high duty cycle */       \
  }

#define I2C1SPM_INIT_DEFAULT                                                    \
  { I2C1,                       /* Use I2C instance 0 */                       \
    gpioPortC,                  /* SCL port */                                 \
    5,                          /* SCL pin */                                  \
    gpioPortC,                  /* SDA port */                                 \
    4,                          /* SDA pin */                                  \
    0,                          /* Location */                                 \
    0,                          /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,      /* Set to standard rate  */                    \
    i2cClockHLRStandard,        /* Set to use 4:4 low/high duty cycle */       \
  }

#define I2CSPM_TRANSFER_TIMEOUT 300000

/** @} (end addtogroup I2CSPM) */


#endif /* INLCUDE_I2CSPMCONFIG_H_ */
