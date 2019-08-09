#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "candrv.h"

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
	/* Chip errata */
	CHIP_Init();

	clockConfig();

	CANInit();

	while (1) {
		CAN_ParseMsg(&g_msgQueue);

		if (CAN0Received == true)
			CAN_Rx(&recvMsg);
	}
}

