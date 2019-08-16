#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_core.h"
#include "em_can.h"
#include "main.h"
#include "candrv.h"
#include "ltc4151drv.h"
#include "crc.h"


#define TX_MSG_OBJ 5
#define RX_MSG_OBJ 6

#define TX_MSG_ID 1
#define RX_MSG_ID 0

#define DLC_8B	8

typedef struct {
	int rxZero;
	int txZero;
} CANState_t;

CAN_MessageObject_TypeDef sendMsg = {0}, recvMsg = {0};

bool msgQueueFull(msgQueue_t *msgQueue)
{
	if (msgQueue->num == QUEUE_LEN)
		return true;
	else
		return false;
}

bool msgQueueEmpty(msgQueue_t *msgQueue)
{
	if (msgQueue->num == 0)
		return true;
	else
		return false;
}

int msgEnqueue(msgQueue_t *msgQueue, CAN_MessageObject_TypeDef *msg)
{
	if (msgQueueFull(msgQueue))
		return -1;

	memcpy((uint8_t *)(&msgQueue->queue[msgQueue->in++]), (uint8_t *)msg, sizeof(*msg));
	if (msgQueue->in == QUEUE_LEN)
		msgQueue->in = 0;

	CORE_CriticalDisableIrq();
	msgQueue->num++;
	CORE_CriticalEnableIrq();

	return msgQueue->num;
}

CAN_MessageObject_TypeDef *msgDequeue(msgQueue_t *msgQueue)
{
	CAN_MessageObject_TypeDef *pcanMsg = NULL;

	if (msgQueueEmpty(msgQueue))
		return pcanMsg;

	pcanMsg = &msgQueue->queue[msgQueue->out++];
	if (msgQueue->out == QUEUE_LEN)
		msgQueue->out = 0;

	CORE_CriticalDisableIrq();
	msgQueue->num--;
	CORE_CriticalEnableIrq();

	return pcanMsg;
}

// BTN0 interrupt flag
static volatile bool btn0Pressed = false; // false;

// BTN1 interrupt flag
static volatile bool btn1Pressed = false;

/***************************************************************************//**
 * @brief initialization of CAN
 *
 * @param can_Device: CAN0
 *
 * @param mode: Operating mode
 ******************************************************************************/
void setUpCAN(CAN_TypeDef *can_Device, CAN_Mode_TypeDef mode)
{
	// Enable the clocks
	CMU_ClockEnable(cmuClock_HFPER, true);
	if (can_Device == CAN0) {
		NVIC_EnableIRQ(CAN0_IRQn);

		// Enable the CANn clock
		CMU_ClockEnable(cmuClock_CAN0, true);

		// Reset the CAN device
		CAN_Reset(can_Device);
		CAN_Enable(can_Device, false);

		// Set the gpio for rx (X) and tx (Y)
		GPIO_PinModeSet(gpioPortC, 0, gpioModeInput, 0);
		GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);

		// Set the ROUTE to match the pins
		CAN_SetRoute(can_Device, true, 0, 0);
	}

	// Reset Message Objects
	CAN_ResetMessages(can_Device, 0);
	CAN_ResetMessages(can_Device, 1);

	// Set the bit timing to get a bitrate of 125 kbit/s
	// The bitrate MUST be chosen based on the configuration of the clock.
	// The desired bitrate might be unreachable depending on the clock frequency.
	CAN_SetBitTiming(can_Device, 500000, 6, 7, 2, 1);

	// Set the CAN device mode
	CAN_SetMode(can_Device, mode);

	// Enable CAN
	CAN_Enable(can_Device, true);
}

/***************************************************************************//**
 *  @brief  CAN message config function
 *
 *  @param can is CAN peripheral
 *  @param message is a message object
 *  @param msgNum is the message object number
 *  @param id is the message id
 *  @param dlc is the message length
 *  @param mask is the message mask
 *  @param tx is true if the message object is used for transmission
 ******************************************************************************/
void configMessageObj(CAN_TypeDef *can,
                      CAN_MessageObject_TypeDef *message,
                      uint8_t msgNum,
                      uint8_t id,
                      uint8_t dlc,
                      uint32_t mask,
                      bool tx)
{
	message->msgNum = msgNum;
	message->id = id;
	message->dlc = dlc;
	message->mask = mask;

	//  CAN_ConfigureMessageObject(can, CAN_TX_IF, message->msgNum,
	//                             true, tx, false, true, true);
	if (tx) {
		// Configure valid, tx/rx, remoteTransfer for the given Message Object
		CAN_ConfigureMessageObject(can, CAN_TX_IF, message->msgNum,
						   true, tx, false, true, true);
	} else {
		// Configure valid, tx/rx, remoteTransfer for the given Message Object
		CAN_ConfigureMessageObject(can, CAN_RX_IF, message->msgNum,
							   true, tx, false, true, true);
		// Set filter to receive any message with any id
		// CAN_SetIdAndFilter(can, CAN_RX_IF, true, message, true);
		CAN_SetIdAndFilter(can, CAN_RX_IF, false, message, true);
	}
}

/***************************************************************************//**
 * @brief CAN_IRQHandlers
 * Interrupt Service Routine for CAN interrupt
 ******************************************************************************/
void CAN0_IRQHandler(void)
{
	uint32_t status = CAN0->STATUS;

	if (status & CAN_STATUS_RXOK) {
		CAN0Received = true;

		status &= ~CAN_STATUS_RXOK;

		CAN0->STATUS = status;
#if 0
		for (int i = 0; i < recvMsg.dlc; i++)
			recvMsg.data[i] = 0;

		CAN_ReadMessage(CAN0, CAN_RX_IF, &recvMsg);
		msgEnqueue(&g_msgQueue, &recvMsg);
#endif
	}

	CAN_MessageIntClear(CAN0, 0xFFFFFFFF);
}

void CAN_Rx(CAN_MessageObject_TypeDef *message)
{
	memset((uint8_t *)message->data, 0x00, DLC_8B);
	CAN_ReadMessage(CAN0, CAN_RX_IF, message);
	msgEnqueue(&g_msgQueue, message);
}

void poll_CAN_Rx(void)
{
	if (CAN0Received == true)
		CAN_Rx(&recvMsg);
}

void CAN_Tx(CAN_MessageObject_TypeDef *message)
{
	CAN_SendMessage(CAN0, CAN_TX_IF, message, true);
}

void poll_CAN_Tx(CAN_MessageObject_TypeDef *canMsg, mainFrame_t *frame)
{
	uint32_t status = 0;

	memcpy(canMsg->data, frame, sizeof(*frame));
	CAN_Tx(canMsg);

	/*
	 * wait TX completely
	 * */
	status = CAN0->STATUS;
	while (!(status & CAN_STATUS_TXOK)) {
		status = CAN0->STATUS;
	}

	/*
	 * clear TXOK status flag
	 * */
	status &= ~CAN_STATUS_TXOK;
	CAN0->STATUS = status;
}

void CANInit(void)
{
	CAN0Received = false;
	memset(&g_msgQueue, 0x00, sizeof(g_msgQueue));
	memset(&sendMsg, 0x00, sizeof(sendMsg));
	memset(&recvMsg, 0x00, sizeof(recvMsg));

	// Initialize CAN peripherals
	CAN_Mode_TypeDef mode =  canModeLoopBack;
	setUpCAN(CAN0, mode);

	//  CAN0->BITTIMING = 0x2301;
	// Initialize a message using 5th Message Object in the RAM to send
	configMessageObj(CAN0, &sendMsg, TX_MSG_OBJ, TX_MSG_ID, DLC_8B, 0, true);

	// Initialize a message using 6th Message Object in the RAM for reception
	configMessageObj(CAN0, &recvMsg, RX_MSG_OBJ, RX_MSG_ID, DLC_8B, 0, false);
}

void handleDaltesterOn(mainFrame_t *frame)
{
	CAN_MessageObject_TypeDef canMsg = {0};
	bool daltesterOnSucc = false;

	frame->type = CTRL_FRAME;

	/*
	 * switch on supply for ballistic tester
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BALTESTER_1, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BALTESTER_2, gpioModeWiredAndPullUpFilter, 1);
	g_baltester_status = BALTESTER_ON;

	if (daltesterOnSucc) {
		frame->cmd_status0 = 0x01;
	} else {
		frame->cmd_status0 = 0xFE;
	}

	canMsg.id = ARB_STS_ID;
	canMsg.dlc = 8;
	memcpy(&canMsg.data, frame, sizeof(*frame));
	CAN_Tx(&canMsg);
}

void handleDaltesterOff(mainFrame_t *frame)
{
	CAN_MessageObject_TypeDef canMsg = {0};
	bool daltesterOffSucc = false;

	frame->type = CTRL_FRAME;

	/*
	 * switch off supply for ballistic tester
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BALTESTER_2, gpioModeWiredAndPullUpFilter, 0);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BALTESTER_1, gpioModeWiredAndPullUpFilter, 0);
	g_baltester_status = BALTESTER_OFF;

	if (daltesterOffSucc) {
		frame->cmd_status0 = 0x02;
	} else {
		frame->cmd_status0 = 0xFD;
	}

	canMsg.id = ARB_STS_ID;
	canMsg.dlc = 8;
	memcpy(&canMsg.data, frame, sizeof(*frame));
	CAN_Tx(&canMsg);
}

void handleBatteryChk(mainFrame_t *frame)
{
	CAN_MessageObject_TypeDef canMsg = {0};
	mainFrame_t mFrame = {0};
	bool batteryChkSucc = true;
	uint16_t serial = (frame->serialHigh << 8) | frame->serialLow;
	uint8_t *pbuf = (uint8_t *)&g_BatteryStatQueue.batteryStatus[g_BatteryStatQueue.latestItem];
	int i = 0;
	uint16_t crc = GetCRC16(pbuf, sizeof(BatteryStatus_t));

	canMsg.id = ARB_STS_ID;
	canMsg.dlc = 8;

	frame->type = CTRL_FRAME;

	/*
	 * safe battery status feedback frame
	 * */
	for (i = 0; i < 9; i++) {
		if (i == 0) { // subFrame 1
			mFrame.subFrameIndex = i;
			mFrame.frameLen = 9;
			mFrame.serialLow = serial & 0xff;
			mFrame.serialHigh = (serial >> 8) & 0xff;
			mFrame.dataLen = 48;
			mFrame.type = STATUS_FRAME;
			poll_CAN_Tx(&canMsg, &mFrame);
		} else if (i >= 1 && i <= 6) { // subFrame 2-7
			memset(&mFrame, 0x00, sizeof(mFrame));
			mFrame.subFrameIndex = i;
			memcpy(&mFrame.frameLen, pbuf + 7 * (i - 1), 7);
			poll_CAN_Tx(&canMsg, &mFrame);
		} else if (i == 7) { // subFrmae 8
			memset(&mFrame, 0x00, sizeof(mFrame));
			mFrame.subFrameIndex = i;
			memcpy(&mFrame.frameLen, pbuf + 7 * (i - 1), 6);
			mFrame.cmd_status1 = crc & 0xff;
			poll_CAN_Tx(&canMsg, &mFrame);
		} else { // subFrame 9
			memset(&mFrame, 0x00, sizeof(mFrame));
			mFrame.subFrameIndex = i;
			mFrame.frameLen = (crc >> 8) & 0xff;
			poll_CAN_Tx(&canMsg, &mFrame);
		}
	}

	if (batteryChkSucc) {
		// TODO
	} else {
		frame->cmd_status0 = 0xFC;
		memcpy(&canMsg.data, frame, sizeof(*frame));
		CAN_Tx(&canMsg);
	}
}

void handlePwrToBattery(mainFrame_t *frame)
{
	CAN_MessageObject_TypeDef canMsg = {0};
	bool pwrToBatterySucc = true;

	frame->type = CTRL_FRAME;

	/*
	 * switch on power from battery
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BATTERY_1, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BATTERY_2, gpioModeWiredAndPullUpFilter, 1);
	g_supply_status = BATTERY_SUPPLY;

	if (pwrToBatterySucc) {
		frame->cmd_status0 = 0x04;

		/*
		 * switch on high power output
		 * */
		GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_1, gpioModeWiredAndPullUpFilter, 1);
		GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_2, gpioModeWiredAndPullUpFilter, 1);
	} else {
		frame->cmd_status0 = 0xFB;
	}

	canMsg.id = ARB_STS_ID;
	canMsg.dlc = 8;
	memcpy(&canMsg.data, frame, sizeof(*frame));
	CAN_Tx(&canMsg);

	/*
	 * update state machine
	 * */
	g_curMode = BATTERYSUPPLY_MODE;
}

void handlePwrToGround(mainFrame_t *frame)
{
	CAN_MessageObject_TypeDef canMsg = {0};
	bool pwrToGroundSucc = true;

	frame->type = CTRL_FRAME;

	/*
	 * switch off power for high power output
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_2, gpioModeWiredAndPullUpFilter, 0);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_1, gpioModeWiredAndPullUpFilter, 0);

	/*
	 * switch off power from battery, automatically switch to power from ground supply.
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BATTERY_2, gpioModeWiredAndPullUpFilter, 0);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BATTERY_1, gpioModeWiredAndPullUpFilter, 0);

	if (pwrToGroundSucc) {
		frame->cmd_status0 = 0x05;
	} else {
		frame->cmd_status0 = 0xFA;
	}

	canMsg.id = ARB_STS_ID;
	canMsg.dlc = 8;
	memcpy(&canMsg.data, frame, sizeof(*frame));
	CAN_Tx(&canMsg);

	/*
	 * update state machine
	 * */
	g_curMode = GROUNDSUPPLY_MODE;
}

void CAN_ParseMsg(msgQueue_t *msgQueue)
{
	CAN_MessageObject_TypeDef *pcanMsg = NULL;
	mainFrame_t recvFrame = {0}, sendFrame = {0};
	uint8_t cmd = 0;

	pcanMsg = msgDequeue(msgQueue);
	if (!pcanMsg)
		return;

	/*
	 * Ignore the message if ARB ID != 0X64
	 * */
	if (pcanMsg->id != ARB_CMD_ID)
		return;

	memcpy((uint8_t *)&recvFrame, pcanMsg->data, pcanMsg->dlc);

	/*
	 * Ignore message with invalid frame type
	 * */
	if (recvFrame.type != CTRL_FRAME && recvFrame.type != STATUS_FRAME)
		return;

	sendFrame.subFrameIndex = 0x00;
	sendFrame.frameLen = 1;
	sendFrame.serialLow = recvFrame.serialLow;
	sendFrame.serialHigh = recvFrame.serialHigh;
	sendFrame.dataLen = 1;

	cmd = recvFrame.cmd_status0;
	switch (cmd & 0xff)
	{
		case CTRL_BALTESTER_ON:
			handleDaltesterOn(&sendFrame);
			break;

		case CTRL_BALTESTER_OFF:
			handleDaltesterOff(&sendFrame);
			break;

		case CTRL_BATSTATUS_CHK:
			handleBatteryChk(&sendFrame);
			break;

		case CTRL_PWR_TO_BATTERY:
			handlePwrToBattery(&sendFrame);
			break;

		case CTRL_PWR_TO_GROUND:
			handlePwrToGround(&sendFrame);
			break;

		default:
			break;
	}
}

void parseForBatterysupplyMode(void)
{
	CAN_ParseMsg(&g_msgQueue);
}

void parseForGroundSupplyMode(void)
{
	CAN_ParseMsg(&g_msgQueue);
}

int8_t configBeforePowerSwitch(void)
{
	int battery_val = 0;

	/*
	 * switch off power for high power output when supply comes from ground supply.
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_2, gpioModeWiredAndPullUpFilter, 0);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_HIGHPOWER_1, gpioModeWiredAndPullUpFilter, 0);

	/*
	 * switch off power from battery, automatically switch to power from ground supply.
	 * */
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BATTERY_2, gpioModeWiredAndPullUpFilter, 0);
	GPIO_PinModeSet(gpioPortC, GPIO_TO_BATTERY_1, gpioModeWiredAndPullUpFilter, 0);

	/*
	 * battery self check
	 * */
	get_Vin(EM_VCC28_HighPowerInputFromBattery_Before, &g_I2CTransferInfo);
	battery_val = g_I2CTransferInfo.rxBuf[2];
	battery_val = (battery_val << 4) | ((g_I2CTransferInfo.rxBuf[3] >> 4) & 0x0f);

	if (battery_val > BATTERY_SANE_CHK_LEVEL)
		return 0;
	else
		return -1;
}

/***************************************************************************//**
 *  @brief  CAN demo function:
 *          two CAN devices needs to be connected
 ******************************************************************************/
void runCANDemo(void)
{
	uint32_t i;

	//EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
	//dcdcInit.anaPeripheralPower = emuDcdcAnaPeripheralPower_AVDD;

	// Chip errata
	//CHIP_Init();

	// Init DCDC regulator
	//EMU_DCDCInit(&dcdcInit);

	//  CMU_HFXOInit(&hfxoInit);
	//CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_Unlock();

	// Initialize CAN_ActionCounter
	CANState_t state;
	state.rxZero = 0, state.txZero = 0;

	// Initialize CAN peripherals
	CAN_Mode_TypeDef mode =  canModeLoopBack;
	setUpCAN(CAN0, mode);

	//  CAN0->BITTIMING = 0x2301;
	// Initialize a message using 5th Message Object in the RAM to send
	CAN_MessageObject_TypeDef message;
	configMessageObj(CAN0, &message, 5, 1, 8, 0, true);

	// Initialize a message using 6th Message Object in the RAM for reception
	CAN_MessageObject_TypeDef receiver;
	configMessageObj(CAN0, &receiver, 6, 0, 8, 0, false);

	// Infinite loop
	while (true) {
		// Send message using CAN if BTN0 is pressed
		if (btn0Pressed) {
			// Send message
			message.id++;
			snprintf((char*)message.data, message.dlc, "BTN0 %ld", message.id);
			CAN_SendMessage(CAN0, CAN_TX_IF, &message, true);
			// Update display of CAN actions
			state.txZero++;
			// Erasing data that has been sent.
			for (i = 0; i < message.dlc; ++i) {
				message.data[i] = 0;
			}
			// Reset flag
			btn0Pressed = false;
		}

		// Send message using CAN if BTN1 is pressed
		if (btn1Pressed) {
			// Send message
			message.id++;
			snprintf((char*)message.data, message.dlc, "BTN1 %ld", message.id);
			CAN_SendMessage(CAN0, CAN_TX_IF, &message, true);

			// Update display of CAN actions
			state.txZero++;

			// Erasing data that has been sent.
			for (i = 0; i < message.dlc; ++i) {
				message.data[i] = 0;
			}
			btn1Pressed = false;
		}
		// Message arrival check on CAN
		if (CAN0Received) {
			// Erasing data to be sure we read what we sent.
			for (i = 0; i < receiver.dlc; ++i) {
				receiver.data[i] = 0;
			}
			CAN_ReadMessage(CAN0, CAN_RX_IF, &receiver);

			state.rxZero++;
			CAN0Received = false;
		}
	}
}
