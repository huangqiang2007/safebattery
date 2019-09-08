#include <stdio.h>
#include <math.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_adc.h"
#include "main.h"
#include "candrv.h"
#include "i2cdrv.h"
#include "ltc4151drv.h"
#include "adcdrv.h"
#include "timer.h"
#include "candrv.h"
#include "i2cspmconfig.h"
#include "i2cspm.h"
#include "pwrDetect.h"

void CAN_test(void)
{
	CAN_MessageObject_TypeDef canMsg = {
		.msgNum = TX_MSG_OBJ,
		.id = ARB_CMD_ID,
		.dlc = DLC_8B,
	};
	mainFrame_t mFrame = {0};

	mFrame.subFrameIndex = 0;
	mFrame.frameLen = 1;
	mFrame.serialLow = 0x01;
	mFrame.serialHigh = 0x00;
	mFrame.dataLen = 1;
	mFrame.type = CTRL_FRAME;

	while (true) {
		poll_CAN_Tx(&canMsg, &mFrame);
	}
}

void GPIO_switch_test(void)
{
	int i = 10;

	while (i--) {
		GPIO_PinModeSet(gpioPortC, 2, gpioModeWiredAndPullUp, 0);
		GPIO_PinModeSet(gpioPortC, 3, gpioModeWiredAndPullUp, 0);

		GPIO_PinModeSet(gpioPortC, 2, gpioModeWiredAndPullUp, 1);
		GPIO_PinModeSet(gpioPortC, 3, gpioModeWiredAndPullUp, 1);

		GPIO_PinModeSet(gpioPortC, 4, gpioModeWiredAndPullUp, 0);
		GPIO_PinModeSet(gpioPortC, 8, gpioModeWiredAndPullUp, 0);

		GPIO_PinModeSet(gpioPortC, 4, gpioModeWiredAndPullUp, 1);
		GPIO_PinModeSet(gpioPortC, 8, gpioModeWiredAndPullUp, 1);

		GPIO_PinModeSet(gpioPortC, 9, gpioModeWiredAndPullUp, 0);
		GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAndPullUp, 0);

		GPIO_PinModeSet(gpioPortC, 9, gpioModeWiredAndPullUp, 1);
		GPIO_PinModeSet(gpioPortC, 10, gpioModeWiredAndPullUp, 1);
	}
}

void I2C_test(void)
{
#if 0
	uint16_t V, I;
	I2CSPM_Init_TypeDef I2C0_init = I2C0SPM_INIT_DEFAULT;
	I2CSPM_Init(&I2C0_init);

	//I2C_reg_write(I2C0, I2C_ADDR_B11_POWER, I2C_REG_LTC4151_CTRL, LTC4151_CMD);

	while (1)
		getB11Power_info(&V, &I);

#else

	//ctrlBattery_gpio
	GPIO_PinModeSet(gpioPortC, 2, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortC, 3, gpioModePushPull, 0);

	GPIO_PinModeSet(gpioPortC, 2, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortC, 3, gpioModePushPull, 1);

	//ctrlpowerOut_Tester
	GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0);

	GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1);

	//highpowerOut_gpio
	GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);

	GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 1);

	GPIO_PinModeSet(gpioPortD, 4, gpioModeWiredAnd, 0);
	GPIO_PinModeSet(gpioPortD, 5, gpioModeWiredAnd, 0);

	GPIO_PinModeSet(gpioPortD, 4, gpioModeWiredAnd, 1);
	GPIO_PinModeSet(gpioPortD, 5, gpioModeWiredAnd, 1);

	while (1) {
		getFloatfromAD(EM_VCC28_HighPowerInputFromBattery_Before, &g_I2CTransferInfo, &ADConvertResult1);
		getFloatfromAD(EM_VCC28_CtrlPowerInputFromBatteryAfterSwitch, &g_I2CTransferInfo, &ADConvertResult1);
		getFloatfromAD(EM_VCC28_CtrlPower_to_Controller, &g_I2CTransferInfo, &ADConvertResult1);
		getFloatfromAD(EM_VCC28_CtrlPower_to_BallisticTester, &g_I2CTransferInfo, &ADConvertResult1);
	}
#endif
}

int main(void)
{
	/*
	 * Chip errata
	 * */
	CHIP_Init();

	/*
	 * Some global info init
	 * */
	globalInit();

	/*
	 * Battery status collect
	 * */
	batteryStatusInit();

	/*
	 * Clock init
	 * */
	clockConfig();

	//while (1);
	/*
	 * Timer init
	 * */
	Timer_init();

	/*
	 * CAN interface init
	 * */
	CANInit(canModeLoopBack);

	CAN_test();

	/*
	 * I2C interfaces init
	 * */
	initI2CIntf();
	//GPIO_switch_test();

	I2C_test();

	/*
	 * Firstly, collect all battery status
	 * */
	batteryStatusCollect(&g_BatteryStatQueue);

	while (1) {
		/*
		 * Collect battery status looping
		 * */
		pollBatteryStatus();

		/*
		 * collect CAN receive information
		 * */
		poll_CAN_Rx();
		switch(g_curMode)
		{
			case IDLE_MODE:
				if (!configBeforePowerSwitch())
					g_curMode = GROUNDSUPPLY_MODE;
				else
					g_curMode = IDLE_MODE;
				break;

			case GROUNDSUPPLY_MODE:
				parseForGroundSupplyMode();
				break;

			case BATTERYSUPPLY_MODE:
				parseForBatterysupplyMode();
				break;

			default:
				g_curMode = IDLE_MODE;
				break;
		}
	}
}
