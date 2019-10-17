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
	 * I2C transfer timeout counter init.
	 * */
	g_timerout_Ticks = 0;

	/*
	 * software watch dog. 2019.09.29 byWH
	 * */
	g_S_WDog_Ticks = g_S_WDog_Ticks_1Hundred;//1s, tick time = 10ms.
	g_S_WDog_SW = g_S_WDog_SW_DISABLE; // Disable the software watch dog function¡£

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

/*
 * calculate temperature from NTC resistor
 * */
float getBatteryTemp(float voltageForRntc)
{
	float Rp = 10000.0; /* divider resistence */
	float B25 = 3950.0; /* coefficient */
	float Ka = 273.15; /* absolute 0 degree kelvin */
	float T2 = (273.15 + 25.0); /* 25 Degree */
	float RefVol = 2500.0; /* refrence voltage */
	float Rntc = 0.0; /* divide resistence */
	float Temp = 0.0; /* temperature */

	Rntc = voltageForRntc * Rp / (RefVol - voltageForRntc);
	Temp = (float)(1 / (((log(Rntc/10000)) / B25) + 1 / T2));
	Temp -= Ka;

	return Temp;
}

/*
 * if battery voltage is lower than such thresh, it indicates
 * the battery needs to charge.
 * */
#define CTRL_BATTERY_LOW_THRESH 27
#define HIGHPWR_BATTERY_LOW_THRESH 27
void updateBatteryStatus(int batteryId, int batVoltage)
{
	if (batteryId == EM_VCC28_CtrlPowerInputFromBatteryAfterSwitch) {
		if (batVoltage < CTRL_BATTERY_LOW_THRESH)
			g_ctrl_battery_status = CTRL_BAT_NEED_CHARG;
		else
			g_ctrl_battery_status = CTRL_BAT_NORMAL;
	} else if (batteryId == EM_VCC28_HighPowerInputFromBattery_Before) {
		if (batVoltage < HIGHPWR_BATTERY_LOW_THRESH)
			g_highpower_status = HIGHPOWER_BAT_NEED_CHARG;
		else
			g_highpower_status = HIGHPOWER_BAT_NORMAL;
	}
}

void batteryStatusCollect(BatteryStatQueue_t *batteryStatQueue)
{
	int8_t index = 0,idx;
	int32_t adVal = 0;
	uint32_t counter;

	index = batteryStatQueue->idx;

	/*
	 * get VCC28_CtrlPowerInputFromGround_Before
	 * */
	if (getFloatfromAD(EM_VCC28_CtrlPowerInputFromGround_Before, &g_I2CTransferInfo, &ADConvertResult1) < 0)
		return;
	batteryStatQueue->batteryStatus[index].groundInputVol = ADConvertResult1.voltage;

	/*
	 * get VCC28_CtrlPowerInputFromBatteryAfterSwitch
	 * */
	if (getFloatfromAD(EM_VCC28_CtrlPowerInputFromBatteryAfterSwitch, &g_I2CTransferInfo, &ADConvertResult1) < 0)
		return;
	batteryStatQueue->batteryStatus[index].ctrlBatteryInputVol = ADConvertResult1.voltage;
	updateBatteryStatus(EM_VCC28_CtrlPowerInputFromBatteryAfterSwitch, ADConvertResult1.voltage);

	/*
	 * get VCC28_HighPowerInputFromBattery_Before
	 * */
	if (getFloatfromAD(EM_VCC28_HighPowerInputFromBattery_Before, &g_I2CTransferInfo, &ADConvertResult1) < 0)
		return;
	batteryStatQueue->batteryStatus[index].highpowerBatteryInputVol = ADConvertResult1.voltage;
	updateBatteryStatus(EM_VCC28_HighPowerInputFromBattery_Before, ADConvertResult1.voltage);

	/*
	 * get VCC28_CtrlPower_to_Controller
	 * */
	if (getFloatfromAD(EM_VCC28_CtrlPower_to_Controller, &g_I2CTransferInfo, &ADConvertResult1) < 0)
		return;
	batteryStatQueue->batteryStatus[index].controllerCtrlOutputVol = ADConvertResult1.voltage;

	/*
	 * get VCC28_CtrlPower_to_BallisticTester
	 * */
	if(getFloatfromAD(EM_VCC28_CtrlPower_to_BallisticTester, &g_I2CTransferInfo, &ADConvertResult2) < 0)
		return;
	batteryStatQueue->batteryStatus[index].baltesterOutputVol = ADConvertResult2.voltage;

	/*
	 * get total control voltage and current
	 * */
	batteryStatQueue->batteryStatus[index].ctrlOutputVol = ADConvertResult1.voltage + ADConvertResult2.voltage;
	//current calculate : they have two resistors in parallel. 2019.10.14@wanhai.
	batteryStatQueue->batteryStatus[index].ctrlOutputCurrent = 2*ADConvertResult1.current + 2*ADConvertResult2.current;

	/*
	 * get VCC28_HighPower_to_Outside
	 * */
	if (getFloatfromAD(EM_VCC28_HighPower_to_Outside, &g_I2CTransferInfo, &ADConvertResult1) < 0)
		return;
	batteryStatQueue->batteryStatus[index].highpowerOutputVol = ADConvertResult1.voltage;
	//current calculate :it have three resistors in parallel. 2019.10.14@wanhai.
	batteryStatQueue->batteryStatus[index].highpowerOutputCurrent = 3*ADConvertResult1.current;

	/*
	 *  battery temperature sample
	 * */
	adVal = 0;
	for(idx = 0; idx < 20; idx++){
		counter = 2000;
		while(counter--);
		adVal += get_AD(adcPosSelAPORT4XCH11);
	}
	batteryStatQueue->batteryStatus[index].ctrlBatteryTemp = getBatteryTemp(adVal*0.05);

	adVal = 0;
	for(idx = 0; idx < 20; idx++){
		counter = 2000;
		while(counter--);
		adVal += get_AD(adcPosSelAPORT4XCH13);
	}
	batteryStatQueue->batteryStatus[index].highpowerBatteryTemp = getBatteryTemp(adVal*0.05);

	/*
	 * Update global status
	 * */
	batteryStatQueue->batteryStatus[index].powerSupplyStatus = g_supply_status;
	batteryStatQueue->batteryStatus[index].ctrlBatteryStatus = g_ctrl_battery_status;
	batteryStatQueue->batteryStatus[index].highPowerBatteryStatus = g_highpower_status;
	batteryStatQueue->batteryStatus[index].baltesterStatus = g_baltester_status;

	batteryStatQueue->latestItem = batteryStatQueue->idx++;
	if (batteryStatQueue->idx == Q_LEN)
		batteryStatQueue->idx = 0;
}

void pollBatteryStatus(void)
{

	/*
	 * Function use condition:
	 * Max time for continuous work of the device must less than MAX_TICK * TickTime.
	 * Avoid counter overflow¡£
	 * Note by 2019.10.17@wanhai.
	 * */
	#define POLL_DELAY 20
	static uint32_t pollTick = 0;

	if (pollTick == 0)
		pollTick = g_Ticks + POLL_DELAY;

	if (g_Ticks < pollTick)
		return;
	/*
	 * set tick for next status collect
	 * */
	pollTick = g_Ticks + POLL_DELAY;

	batteryStatusCollect(&g_BatteryStatQueue);
}
