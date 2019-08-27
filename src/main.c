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
	poll_CAN_Tx(&canMsg, &mFrame);
}

void I2C_test(void)
{
	int i = 3;

	while (i--)
		get_Vin(EM_VCC28_CtrlPowerInputFromGround_Before, &g_I2CTransferInfo);
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

	/*
	 * Timer init
	 * */
	Timer_init();

	/*
	 * CAN interface init
	 * */
	CANInit(canModeLoopBack);

	//CAN_test();

	/*
	 * I2C interfaces init
	 * */
	initI2CIntf();

	//I2C_test();

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
