#ifndef __MAIN_H_
#define __MAIN_H_

/*
 * power supply status
 * */
volatile uint8_t g_supply_status;
enum {GROUND_SUPPLY=0x10, BATTERY_SUPPLY=0x02};

/*
 * control battery status
 * */
volatile uint8_t g_ctrl_battery_status;
enum {CTRL_BAT_NORMAL=0x10, CTRL_BAT_NEED_CHARG=0x20, CTRL_BAT_IN_CHARGING=0x11};

/*
 * high power battery status
 * */
volatile uint8_t g_highpower_status;
enum {HIGHPOWER_BAT_NORMAL=0x10, HIGHPOWER_BAT_NEED_CHARG=0x20, HIGHPOWER_BAT_IN_CHARGING=0x11};

/*
 * ballistic tester status
 * */
volatile uint8_t g_baltester_status;
enum {BALTESTER_OFF=0x10, BALTESTER_ON=0x20};

typedef struct {
	float current;
	float voltage;
} ADConvertResult_t;

ADConvertResult_t ADConvertResult1, ADConvertResult2;

/*
 * battery status feedback information
 * */
typedef struct {
	uint8_t powerSupplyStatus;
	uint8_t ctrlBatteryStatus;
	uint8_t highPowerBatteryStatus;
	uint8_t baltesterStatus;
	float groundInputVol;
	float ctrlBatteryInputVol;
	float highpowerBatteryInputVol;
	float ctrlOutputVol; // controllerCtrlOutputVol + baltesterOutputVol
	float ctrlOutputCurrent;
	float ctrlBatteryTemp;
	float highpowerOutputVol;
	float highpowerOutputCurrent;
	float highpowerBatteryTemp;
	float controllerCtrlOutputVol;
	float baltesterOutputVol;
} __attribute__((packed)) BatteryStatus_t;

#define Q_LEN 3
typedef struct {
	int8_t idx;
	int8_t latestItem;
	BatteryStatus_t batteryStatus[Q_LEN];
} BatteryStatQueue_t;

BatteryStatQueue_t g_BatteryStatQueue;

/*
 * state machine
 * */
enum {IDLE_MODE, GROUNDSUPPLY_MODE, BATTERYSUPPLY_MODE};

volatile uint8_t g_curMode;

#endif
