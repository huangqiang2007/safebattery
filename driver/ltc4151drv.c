#include "em_i2c.h"
#include "i2cdrv.h"

/*
 * (I2C index, I2C slave addr, I2C read_write flag, tx len, rx len)
 * */
uint8_t I2C_MonitorObjs[6][5] = {
		{0, 0xDE, I2C_FLAG_WRITE_READ, 1, 2},
		{0, 0xCE, I2C_FLAG_WRITE_READ, 1, 2},
		{0, 0xDE, I2C_FLAG_WRITE_READ, 1, 2},
		{0, 0xDE, I2C_FLAG_WRITE_READ, 1, 2},
		{0, 0xCE, I2C_FLAG_WRITE_READ, 1, 2},
		{0, 0xDE, I2C_FLAG_WRITE_READ, 1, 2},
};

/*
 * get Vin AD sample
 * */
void get_Vin(uint8_t which_ad, I2CTransferInfo_t *I2CTransferInfo)
{
	I2C_TypeDef *i2c = NULL;

	memset(I2CTransferInfo, 0x00, sizeof(*I2CTransferInfo));
	I2CTransferInfo->i2cIdx = I2C_MonitorObjs[which_ad][0];
	I2CTransferInfo->i2cSlaveAddr = I2C_MonitorObjs[which_ad][1];
	I2CTransferInfo->i2cRdwrFlag = I2C_MonitorObjs[which_ad][2];
	I2CTransferInfo->txLen = I2C_MonitorObjs[which_ad][3];
	I2CTransferInfo->txBuf[0] = 0x02;
	I2CTransferInfo->rxLen = I2C_MonitorObjs[which_ad][4];

	if (I2CTransferInfo->i2cIdx == 0)
		i2c = I2C0;
	else if (I2CTransferInfo->i2cIdx == 1)
		i2c = I2C1;

	performI2CTransfer(i2c, I2CTransferInfo);
}
