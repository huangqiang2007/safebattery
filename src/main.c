#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "main.h"
#include "candrv.h"
#include "i2cdrv.h"
#include "ltc4151drv.h"

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

void batteryStatusInit(void)
{
	memset(&ADConvertResult1, 0x00, sizeof(ADConvertResult1));
	memset(&ADConvertResult2, 0x00, sizeof(ADConvertResult2));
	memset(&g_BatteryStatQueue, 0x00, sizeof(BatteryStatQueue_t));
	g_supply_status = 0;
	g_ctrl_battery_status = 0;
	g_highpower_status = 0;
	g_baltester_status = 0;
}

void batteryStatusCollect(BatteryStatQueue_t *batteryStatQueue)
{
	int8_t index = 0;
	int val = 0;

	batteryStatQueue->latestItem = batteryStatQueue->idx;
	if (batteryStatQueue->idx == Q_LEN - 1)
		batteryStatQueue->idx = 0;

	index = batteryStatQueue->idx;

	batteryStatQueue->batteryStatus[index].powerSupplyStatus = g_supply_status;
	batteryStatQueue->batteryStatus[index].ctrlBatteryStatus = g_ctrl_battery_status;
	batteryStatQueue->batteryStatus[index].highPowerBatteryStatus = g_highpower_status;
	batteryStatQueue->batteryStatus[index].baltesterStatus = g_baltester_status;

	/*
	 * get VCC28_CtrlPowerInputFromGround_Before
	 * */
	getFloatfromAD(EM_VCC28_CtrlPowerInputFromGround_Before, &g_I2CTransferInfo, &ADConvertResult1);
	batteryStatQueue->batteryStatus[index].groundInputVol = ADConvertResult1.voltage;

	/*
	 * get VCC28_CtrlPowerInputFromBatteryAfterSwitch
	 * */
	getFloatfromAD(EM_VCC28_CtrlPowerInputFromBatteryAfterSwitch, &g_I2CTransferInfo, &ADConvertResult1);
	batteryStatQueue->batteryStatus[index].ctrlBatteryInputVol = ADConvertResult1.voltage;

	/*
	 * get VCC28_HighPowerInputFromBattery_Before
	 * */
	getFloatfromAD(EM_VCC28_HighPowerInputFromBattery_Before, &g_I2CTransferInfo, &ADConvertResult1);
	batteryStatQueue->batteryStatus[index].highpowerBatteryInputVol = ADConvertResult1.voltage;

	/*
	 * get VCC28_CtrlPower_to_Controller
	 * */
	getFloatfromAD(EM_VCC28_CtrlPower_to_Controller, &g_I2CTransferInfo, &ADConvertResult1);
	batteryStatQueue->batteryStatus[index].controllerCtrlOutputVol = ADConvertResult1.voltage;

	/*
	 * get VCC28_CtrlPower_to_BallisticTester
	 * */
	getFloatfromAD(EM_VCC28_CtrlPower_to_BallisticTester, &g_I2CTransferInfo, &ADConvertResult2);
	batteryStatQueue->batteryStatus[index].baltesterOutputVol = ADConvertResult2.voltage;

	/*
	 * get total control voltage and current
	 * */
	batteryStatQueue->batteryStatus[index].ctrlOutputVol = ADConvertResult1.voltage + ADConvertResult2.voltage;
	batteryStatQueue->batteryStatus[index].ctrlOutputCurrent = ADConvertResult1.current + ADConvertResult2.current;

	/*
	 * get VCC28_HighPower_to_Outside
	 * */
	getFloatfromAD(EM_VCC28_HighPower_to_Outside, &g_I2CTransferInfo, &ADConvertResult1);
	batteryStatQueue->batteryStatus[index].highpowerOutputVol = ADConvertResult1.voltage;
	batteryStatQueue->batteryStatus[index].highpowerOutputCurrent = ADConvertResult1.current;

	// TODO battery temperature sample

}

int main(void)
{
	/* Chip errata */
	CHIP_Init();

	g_curMode = IDLE_MODE;

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

	while (1) {
		if (CAN0Received == true)
			CAN_Rx(&recvMsg);

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
