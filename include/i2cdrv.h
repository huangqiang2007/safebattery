#ifndef __I2CDRV_H_
#define __I2CDRV_H_

typedef struct {
	uint8_t i2cIdx;
	uint8_t i2cSlaveAddr;
	uint8_t i2cRdwrFlag;
	uint8_t txBuf[4];
	uint8_t txLen;
	uint8_t rxBuf[6];
	uint8_t rxLen;
} I2CTransferInfo_t;

I2CTransferInfo_t g_I2CTransferInfo;

void initI2C(int8_t i2cIdx);
void initI2CIntf(void);
void performI2CTransfer(I2C_TypeDef *i2c, I2CTransferInfo_t *pI2CTransferInfo);

#endif
