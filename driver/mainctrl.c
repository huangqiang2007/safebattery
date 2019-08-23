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

void globalInit(void)
{
	g_curMode = IDLE_MODE;
	g_Ticks = 0;

	/*
	 * Init, CAN relative
	 * */
	CAN0Received = false;
	memset(&g_msgQueue, 0x00, sizeof(g_msgQueue));
	memset(&sendMsg, 0x00, sizeof(sendMsg));

	/*
	 * Init, I2C relative
	 * */
	memset(&g_I2CTransferInfo, 0x00, sizeof(g_I2CTransferInfo));
}

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
	g_supply_status = GROUND_SUPPLY;
	g_ctrl_battery_status = CTRL_BAT_NORMAL;
	g_highpower_status = HIGHPOWER_BAT_NORMAL;
	g_baltester_status = BALTESTER_OFF;
}

float getBatteryTemp(float voltageForRntc)
{
	float Rntc = 0.0, Temp = 0.0;
	int B25 = 3950;

	Rntc = voltageForRntc * 10000 / (3300 - voltageForRntc);

	Temp = 1 / (((log(Rntc) - log(10000)) / B25) + 1 / 25);

	return Temp;
}

void batteryStatusCollect(BatteryStatQueue_t *batteryStatQueue)
{
	int8_t index = 0;

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

	/*
	 *  battery temperature sample
	 * */
	batteryStatQueue->batteryStatus[index].ctrlBatteryTemp = getBatteryTemp(get_AD(adcPosSelAPORT4XCH11));
	batteryStatQueue->batteryStatus[index].highpowerBatteryTemp = getBatteryTemp(get_AD(adcPosSelAPORT4XCH13));
}

void pollBatteryStatus(void)
{
	#define POLL_DELAY 500
	static uint32_t pollTick = 0;

	if (pollTick == 0)
		pollTick = g_Ticks + POLL_DELAY;

	if (g_Ticks < pollTick)
		return;

	batteryStatusCollect(&g_BatteryStatQueue);

	/*
	 * set tick for next status collect
	 * */
	pollTick = g_Ticks + POLL_DELAY;
}
