#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_can.h"

// CAN bus interfaces
#define CAN_TX_IF                 0
#define CAN_RX_IF                 1

typedef struct {
	int rxZero;
	int txZero;
} CANState_t;


// BTN0 interrupt flag
static volatile bool btn0Pressed = false; // false;

// BTN1 interrupt flag
static volatile bool btn1Pressed = false;

// message arrival flag on CAN
static volatile bool CAN0Received = false;

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
		GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 0);
		GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);

		// Set the ROUTE to match the pins
		CAN_SetRoute(can_Device, true, 2, 2);
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
 *  @brief  CAN demo function:
 *          two CAN devices needs to be connected
 ******************************************************************************/
void runCANDemo(void)
{
	uint32_t i;

	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
	dcdcInit.anaPeripheralPower = emuDcdcAnaPeripheralPower_AVDD;

	// Chip errata
	CHIP_Init();

	// Init DCDC regulator
	EMU_DCDCInit(&dcdcInit);

	//  CMU_HFXOInit(&hfxoInit);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_Unlock();

	// Initialize CAN_ActionCounter
	CANState_t state;
	state.rxZero = 0, state.txZero = 0;

	// Initialize CAN peripherals
	CAN_Mode_TypeDef mode =  canModeLoopBack; //
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

/***************************************************************************//**
 * @brief CAN_IRQHandlers
 * Interrupt Service Routine for CAN interrupt
 ******************************************************************************/
void CAN0_IRQHandler(void)
{
	if (CAN0->STATUS & CAN_STATUS_RXOK) {
		CAN0Received = true;
	}

	CAN0->STATUS = 0x0;
	CAN_MessageIntClear(CAN0, 0xFFFFFFFF);
}

int main(void)
{
	/* Chip errata */
	CHIP_Init();

	/* Infinite loop */
	while (1) {
	}
}

