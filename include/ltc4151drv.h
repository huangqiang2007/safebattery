#ifndef __LTC4151_H_
#define __LTC4151_H_

/*
 * enum for Vin sample
 * */
enum {
	EM_VCC28_CtrlPowerInputFromGround_Before = 0,
	EM_VCC28_CtrlPowerInputFromBatteryAfterSwitch,
	EM_VCC28_HighPowerInputFromBattery_Before,
	EM_VCC28_CtrlPower_to_Controller,
	EM_VCC28_CtrlPower_to_BallisticTester,
	EM_VCC28_HighPower_to_Outside,
};

/*
 * if battery voltage is lower than 25V, battery supply is abnormal.
 * 25V ~= 1000, 12 bit AD sample resolution
 * 28V ~= 1120
 * */
#define BATTERY_SANE_CHK_LEVEL 1000

void get_Vin(uint8_t which_ad, I2CTransferInfo_t *I2CTransferInfo);

#endif
