#include <stdio.h>
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

int main(void)
{
	/* Chip errata */
	CHIP_Init();

	/*
	 * Some global info init
	 * */
	globalInit();

	/*
	 * Timer init
	 * */
	Timer_init();

	/*
	 * Battery status collect
	 * */
	batteryStatusInit();

	/*
	 * Clock init
	 * */
	clockConfig();

	/*
	 * CAN interface init
	 * */
	CANInit();

	/*
	 * I2C interfaces init
	 * */
	initI2CIntf();

	/*
	 * Firstly, collect all battery status
	 * */
	batteryStatusCollect(&g_BatteryStatQueue);

	while (1) {
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

		/*
		 * Collect battery status looping
		 * */
		pollBatteryStatus();
	}
}
