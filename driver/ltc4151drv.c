#include "em_i2c.h"
#include "main.h"
#include "i2cdrv.h"

/*
 * (I2C index, I2C slave addr, I2C read_write flag, tx len, rx len)
 * */
uint8_t I2C_MonitorObjs[7][5] = {
		{0, 0xDE, I2C_FLAG_WRITE_READ, 1, 4},
		{0, 0xD8, I2C_FLAG_WRITE_READ, 1, 4},
		{0, 0xD4, I2C_FLAG_WRITE_READ, 1, 4},
		{0, 0xCE, I2C_FLAG_WRITE_READ, 1, 4},
		{0, 0xD2, I2C_FLAG_WRITE_READ, 1, 4},
		{1, 0xDE, I2C_FLAG_WRITE_READ, 1, 4},
		{1, 0xD8, I2C_FLAG_WRITE_READ, 1, 4}
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
	I2CTransferInfo->txBuf[0] = 0x00;
	I2CTransferInfo->rxLen = I2C_MonitorObjs[which_ad][4];

	if (I2CTransferInfo->i2cIdx == 0)
		i2c = I2C0;
	else if (I2CTransferInfo->i2cIdx == 1)
		i2c = I2C1;
	else{
		// do nothing
		return;
	}

	performI2CTransfer(i2c, I2CTransferInfo);
}

int getFloatfromAD(uint8_t which_ad, I2CTransferInfo_t *I2CTransferInfo, ADConvertResult_t *ADConvertResult)
{
	int val = 0;

	get_Vin(which_ad, I2CTransferInfo);

	val = g_I2CTransferInfo.rxBuf[0];
	val = (val << 4) | ((g_I2CTransferInfo.rxBuf[1] >> 4) & 0x0f);
//	if (val >= 0xfff || val == 0)
//		return -1;

	ADConvertResult->current = (float)val *0.002; // val * 20uV / 0.01�� = val * 0.002 ;

	val = g_I2CTransferInfo.rxBuf[2];
	val = (val << 4) | ((g_I2CTransferInfo.rxBuf[3] >> 4) & 0x0f);
	//if (val >= 0xfff || val == 0)
	//	return -1;

	ADConvertResult->voltage = (float)((val * 102400) >> 12) / 1000; // ((val *102.4 * 1000) >> 12) / 1000

	return 0;
}
