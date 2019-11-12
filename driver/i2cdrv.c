/**************************************************************************//**
 * @file main_tg11.c
 * @brief This project demonstrates both master and slave configurations of the
 * EFM32 I2C peripheral. Two EFM32 I2C modules are connected and set up
 * to both transmit (master mode) and receive (slave mode) between each
 * other using a common I2C bus.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "i2cdrv.h"
#include "timer.h"

#define I2C_CLK 10000

// Defines
#define CORE_FREQUENCY              14000000
#define RTC_MIN_TIMEOUT                32000
#define I2C_ADDRESS                     0xE2
#define I2C_RXBUFFER_SIZE                 10

#define timeoutTicks3  3
#define timeoutTicks5  5

// Buffers++
uint8_t i2c_txBuffer[] = "Gecko";
uint8_t i2c_txBufferSize = sizeof(i2c_txBuffer);
uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE];
uint8_t i2c_rxBufferIndex;

// Transmission flags
volatile bool i2c_rxInProgress;
volatile bool i2c_startTx;

/**************************************************************************//**
 * @brief  Starting oscillators and enabling clocks
 *****************************************************************************/
void initCMU(void)
{
	// Enabling clock to the I2C, GPIO, LE
	CMU_ClockEnable(cmuClock_I2C0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_HFLE, true);

	// Starting LFXO and waiting until it is stable
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
}

/**************************************************************************//**
 * @brief  enables I2C slave interrupts
 *****************************************************************************/
void enableI2cSlaveInterrupts(void)
{
  I2C_IntClear(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  I2C_IntEnable(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  NVIC_EnableIRQ(I2C0_IRQn);
}

/**************************************************************************//**
 * @brief  disables I2C interrupts
 *****************************************************************************/
void disableI2cInterrupts(void)
{
	NVIC_DisableIRQ(I2C0_IRQn);
	I2C_IntDisable(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
	I2C_IntClear(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
}

/**************************************************************************//**
 * @brief  Setup I2C
 *****************************************************************************/
void initI2C(int8_t i2cIdx)
{
	I2C_TypeDef *i2c = NULL;

	// Using default settings
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	// Use ~400khz SCK
	i2cInit.freq = 4600;//I2C_CLK; //I2C_FREQ_FAST_MAX;
	i2cInit.refFreq = 0;
	i2cInit.clhr = _I2C_CTRL_CLHR_ASYMMETRIC;
	CMU_ClockEnable(cmuClock_GPIO, true);

	if (i2cIdx == 0) {
		i2c = I2C0;
		CMU_ClockEnable(cmuClock_I2C0, true);

		// Using PD6 (SDA) and PD7 (SCL)
		GPIO_PinModeSet(gpioPortD, 6, gpioModeWiredAnd, 1);
		GPIO_PinModeSet(gpioPortD, 7, gpioModeWiredAndPullUp, 1);

		i2c->ROUTEPEN = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
		i2c->ROUTELOC0 = (i2c->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK)) | I2C_ROUTELOC0_SDALOC_LOC1;
		i2c->ROUTELOC0 = (i2c->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK)) | I2C_ROUTELOC0_SCLLOC_LOC1;
	} else if (i2cIdx == 1) {
		i2c = I2C1;
		CMU_ClockEnable(cmuClock_I2C1, true);

		// Using PD4 (SDA) and PD5 (SCL)
		GPIO_PinModeSet(gpioPortD, 4, gpioModeWiredAnd, 1);
		GPIO_PinModeSet(gpioPortD, 5, gpioModeWiredAndPullUp, 1);

		i2c->ROUTEPEN = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
		i2c->ROUTELOC0 = (i2c->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK)) | I2C_ROUTELOC0_SDALOC_LOC3;
		i2c->ROUTELOC0 = (i2c->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK)) | I2C_ROUTELOC0_SCLLOC_LOC3;
	}

	// Initializing the I2C
	I2C_Init(i2c, &i2cInit);

	// Setting up to enable slave mode
	//i2c->SADDR = 0xDE;
	//i2c->CTRL |= I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;
	//enableI2cSlaveInterrupts();
}

void initI2CIntf(void)
{
	initI2C(0);
	initI2C(1);
}

/**************************************************************************//**
 * @brief  Transmitting I2C data. Will busy-wait until the transfer is complete.
 *****************************************************************************/
void performI2CTransfer(I2C_TypeDef *i2c, I2CTransferInfo_t *pI2CTransferInfo)
{
	// Transfer structure
	I2C_TransferSeq_TypeDef i2cTransfer;
	I2C_TransferReturn_TypeDef result;

	// Initializing I2C transfer
	i2cTransfer.addr          = pI2CTransferInfo->i2cSlaveAddr;
	i2cTransfer.flags         = pI2CTransferInfo->i2cRdwrFlag; // I2C_FLAG_WRITE_READ;
	i2cTransfer.buf[0].data   = pI2CTransferInfo->txBuf;
	i2cTransfer.buf[0].len    = pI2CTransferInfo->txLen;
	i2cTransfer.buf[1].data   = pI2CTransferInfo->rxBuf;
	i2cTransfer.buf[1].len    = pI2CTransferInfo->rxLen;
#if 1
	result = I2C_TransferInit(i2c, &i2cTransfer);

	// Sending data
	g_timerout_Ticks = 0; // reset timeout tick, 10ms per-tick.
	while (result == i2cTransferInProgress && g_timerout_Ticks < timeoutTicks3) {
		result = I2C_Transfer(i2c);
	}

	if (g_timerout_Ticks >= timeoutTicks3 || result == i2cTransferUsageFault \
			|| result == i2cTransferArbLost || result == i2cTransferBusErr \
			|| result == i2cTransferSwFault) {
		I2C_Reset(i2c);
		if (i2c == I2C0) {
			initI2C(0);
		}
		else if (i2c == I2C1) {
			initI2C(1);
		}
	}

#else
	I2CSPM_Transfer(i2c, &i2cTransfer);
#endif
	//enableI2cSlaveInterrupts();
}

/**************************************************************************//**
 * @brief  Receiving I2C data. Along with the I2C interrupt, it will keep the
  EFM32 in EM1 while the data is received.
 *****************************************************************************/
void receiveI2CData(void)
{
	while(i2c_rxInProgress) {
		EMU_EnterEM1();
	}
}

/**************************************************************************//**
 * @brief I2C Interrupt Handler.
 *        The interrupt table is in assembly startup file startup_efm32.s
 *****************************************************************************/
void I2C0_IRQHandler(void)
{
	int status;

	status = I2C0->IF;

	if (status & I2C_IF_ADDR) {
		// Address Match
		// Indicating that reception is started
		i2c_rxInProgress = true;
		I2C0->RXDATA;
		i2c_rxBufferIndex = 0;

		I2C_IntClear(I2C0, I2C_IFC_ADDR);
	} else if (status & I2C_IF_RXDATAV) {
		// Data received
		i2c_rxBuffer[i2c_rxBufferIndex] = I2C0->RXDATA;
		i2c_rxBufferIndex++;
	}

	if(status & I2C_IEN_SSTOP) {
		// Stop received, reception is ended
		I2C_IntClear(I2C0, I2C_IEN_SSTOP);
		i2c_rxInProgress = false;
		i2c_rxBufferIndex = 0;
	}
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	// Clear pending
	uint32_t interruptMask = GPIO_IntGet();
	GPIO_IntClear(interruptMask);

	// If RX is not in progress, a new transfer is started
	if (!i2c_rxInProgress) {
		disableI2cInterrupts();
		i2c_startTx = true;
	}
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int i2cMain(void)
{
  // Chip errata
  CHIP_Init();

  // Configuring clocks in the Clock Management Unit (CMU)
  initCMU();

  // Setting up i2c
  initI2C(0);

  while (1)
  {
    if(i2c_rxInProgress)
    {
       // Receiving data
       receiveI2CData();
    }else if (i2c_startTx)
    {
       // Transmitting data
       //performI2CTransfer();
       // Transmission complete
       i2c_startTx = false;
    }

    // Forever enter EM2. The RTC or I2C will wake up the EFM32
    EMU_EnterEM2(false);
  }
}
