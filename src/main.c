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

void CAN_test(void)
{
	CAN_MessageObject_TypeDef canMsg = {
		.msgNum = TX_MSG_OBJ,
		.id = ARB_STS_ID,
		.dlc = DLC_8B,
	};
	mainFrame_t mFrame = {0};

	mFrame.subFrameIndex = 0;
	mFrame.frameLen = 1;
	mFrame.serialLow = 0x01;
	mFrame.serialHigh = 0x00;
	mFrame.dataLen = 1;
	mFrame.type = CTRL_FRAME;

	memset(&mFrame.subFrameIndex, 0xf0, 8);

	while (true) {
		poll_CAN_Tx(&canMsg, &mFrame);
	}
}

void GPIO_switch_test(void)
{
	int i = 10;

	while (1) {
		GPIO_PinModeSet(gpioPortC, 2, gpioModePushPull, 0);
		GPIO_PinModeSet(gpioPortC, 3, gpioModePushPull, 0);

		GPIO_PinModeSet(gpioPortC, 2, gpioModePushPull, 1);
		GPIO_PinModeSet(gpioPortC, 3, gpioModePushPull, 1);

		GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 0);
		GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0);

		GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 1);
		GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1);

		GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 0);
		GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);

		GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1);
		GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 1);
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


	GPIO_PinModeSet(gpioPortC, GPIO_TO_CTRLPOWER_AD, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_AD, gpioModePushPull, 1);

	while (1) {
//		delayms(100);
		memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult_t));
		getFloatfromAD(EM_VCC28_CtrlPowerInputFromGround_Before, &g_I2CTransferInfo, &ADConvertResult1);

		memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult_t));
		getFloatfromAD(EM_VCC28_CtrlPowerInputFromBatteryAfterSwitch, &g_I2CTransferInfo, &ADConvertResult1);

		memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult_t));
		getFloatfromAD(EM_VCC28_CtrlPowerInputFromBattery_Before, &g_I2CTransferInfo, &ADConvertResult1);

		memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult_t));
		getFloatfromAD(EM_VCC28_CtrlPower_to_Controller, &g_I2CTransferInfo, &ADConvertResult1);

		memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult_t));
		getFloatfromAD(EM_VCC28_CtrlPower_to_BallisticTester, &g_I2CTransferInfo, &ADConvertResult1);

		memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult_t));
		getFloatfromAD(EM_VCC28_HighPowerInputFromBattery_Before, &g_I2CTransferInfo, &ADConvertResult1);

		memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult_t));
		getFloatfromAD(EM_VCC28_HighPower_to_Outside, &g_I2CTransferInfo, &ADConvertResult1);
	}
#endif
}

void highPwrCtrl(void)
{
	ADConvertResult_t ADConvertResult;
	uint32_t timeOut = g_Ticks + 100;

	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_1, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_2, gpioModePushPull, 1);

	/*
	 * delay 1s to switch off high power input.
	 * */
	while (timeOut < g_Ticks) {
		/*
		* get VCC28_HighPowerInputFromBattery_Before
		* */
		if (getFloatfromAD(EM_VCC28_HighPowerInputFromBattery_Before, &g_I2CTransferInfo, &ADConvertResult) < 0)
			return;
	}
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_1, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_2, gpioModePushPull, 0);
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
//	GPIO_switch_test();

	/*
	 * Timer init
	 * */
	Timer_init();

	/*
	 * CAN interface init
	 * */
	CANInit(canModeNormal);

	//CAN_test();

	/*
	 * I2C interfaces init
	 * */
	initI2CIntf();

//	I2C_test();

	/*
	 * Cut off battery supply and highpower supply
	 * */
	configBeforePowerSwitch();

	/*
	 *Open the AD chip to collect Ctrlpower and Highpower voltages of battery.
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_CTRLPOWER_AD, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_AD, gpioModePushPull, 1);

	/*
	 * Firstly, collect all battery status
	 * */
	batteryStatusCollect(&g_BatteryStatQueue);

	while (1) {
		/*
		 * Collect battery status looping
		 * */
		pollBatteryStatus();

		switch(g_curMode)
		{
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

		g_S_WDog_Ticks = g_S_WDog_Ticks_1Hundred; // feed software watch dog.
	}
}
