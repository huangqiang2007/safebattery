#ifndef __CANDRV_H_
#define __CANDRV_H_
#include "em_can.h"

#define CAN_TX_IF 0
#define CAN_RX_IF 1

/*
 * CAN ARB IDs
 * */
enum {ARB_STS_ID = 0x46, ARB_CMD_ID = 0x64};

/*
 * CAN frame type
 * */
enum {CTRL_FRAME = 0x01, STATUS_FRAME = 0x02};

/*
 * Ctrl commands
 * */
#define CTRL_BALTESTER_ON 0x01
#define CTRL_BALTESTER_OFF 0x02
#define CTRL_BATSTATUS_CHK 0x03
#define CTRL_PWR_TO_BATTERY 0x04
#define CTRL_PWR_TO_GROUND 0x05

/*
 * Status feedback for Ctrl commands
 * */
#define STS_BALTESTER_PWRON_OK 0x01
#define STS_BALTESTER_PWRON_FAIL 0xFE
#define STS_BALTESTER_PWROFF_OK 0x02
#define STS_BALTESTER_PWROFF_FAIL 0xFD
#define STS_BATSTATUS_CHK_OK 0x03
#define STS_BATSTATUS_CHK_FAIL 0xFC
#define CTRL_PWR_TO_BATTERY_OK 0x04
#define CTRL_PWR_TO_BATTERY_FAIL 0xFB
#define CTRL_PWR_TO_GROUND_OK 0x05
#define CTRL_PWR_TO_GROUND_FAIL 0xFA

/*
 * GPIO ctrl pins
 * */
#define GPIO_TO_BATTERY_1 2
#define GPIO_TO_BATTERY_2 3
#define GPIO_TO_BALTESTER_1 4
#define GPIO_TO_BALTESTER_2 8
#define GPIO_TO_HIGHPOWER_1 9
#define GPIO_TO_HIGHPOWER_2 10

/*
 * main frame for Ctrl commands and corresponding feedback
 * */
typedef struct {
	uint8_t subFrameIndex;
	uint8_t frameLen;
	uint8_t serialLow;
	uint8_t serialHigh;
	uint8_t dataLen;
	uint8_t type;
	uint8_t cmd_status0;
	uint8_t cmd_status1;
} mainFrame_t;

/*
 * sub frame for battery status feedback
 * */
typedef struct {
	uint8_t subFrameIndex;
	uint8_t data[7];
} subFrame_t;

#define QUEUE_LEN 5
typedef struct {
	int8_t num;
	int8_t in;
	int8_t out;
	CAN_MessageObject_TypeDef queue[QUEUE_LEN];
} msgQueue_t;

msgQueue_t g_msgQueue;

extern CAN_MessageObject_TypeDef sendMsg, recvMsg;
volatile bool CAN0Received;

extern void runCANDemo(void);
extern void CANInit(void);
extern void CAN_Rx(CAN_MessageObject_TypeDef *message);
extern void poll_CAN_Rx(void);
extern void CAN_ParseMsg(msgQueue_t *msgQueue);
void parseForBatterysupplyMode(void);
void parseForGroundSupplyMode(void);
int8_t configBeforePowerSwitch(void);

#endif
