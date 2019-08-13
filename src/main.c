#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "main.h"
#include "candrv.h"
#include "i2cdrv.h"

void clockConfig(void)
{
	SystemCoreClockUpdate();

	/*
	 * chose external crystal oscillator as clock source.
	 * */
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

	/*
	 * Enable clocks required
	 * */
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
}

int main(void)
{
	g_curMode = IDLE_MODE;

	/* Chip errata */
	CHIP_Init();

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

	while (1) {
		CAN_ParseMsg(&g_msgQueue);

		if (CAN0Received == true)
			CAN_Rx(&recvMsg);

		switch(g_curMode)
		{
			case IDLE_MODE:
				g_curMode = GROUNDSUPPLY_MODE;
				break;

			case GROUNDSUPPLY_MODE:
				break;

			case BATTERYSUPPLY_MODE:
				break;

			default:
				g_curMode = IDLE_MODE;
				break;
		}
	}
}
