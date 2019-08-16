#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"

#define adcFreq   16000000

volatile uint32_t sample;
volatile uint32_t millivolts;

/**************************************************************************//**
 * @brief  Initialize ADC function
 *****************************************************************************/
void initADC (uint32_t adchl)
{
	// Enable ADC0 clock
	CMU_ClockEnable(cmuClock_ADC0, true);

	// Declare init structs
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

	// Modify init structs and initialize
	init.prescale = ADC_PrescaleCalc(adcFreq, 0); // Init to max ADC clock for Series 1

	initSingle.diff       = false;        // single ended
	initSingle.reference  = adcRef2V5;    // internal 2.5V reference
	initSingle.resolution = adcRes12Bit;  // 12-bit resolution
	initSingle.acqTime    = adcAcqTime4;  // set acquisition time to meet minimum requirement

	// Select ADC input. See README for corresponding EXP header pin.
	initSingle.posSel = adchl; // adcPosSelAPORT0XCH2;
	init.timebase = ADC_TimebaseCalc(0);

	ADC_Init(ADC0, &init);
	ADC_InitSingle(ADC0, &initSingle);
}

float get_AD(uint32_t adchl)
{
	int sample = 0;
	float temp = 0.0;

	// adcPosSelAPORT4XCH11
	initADC(adchl);

	// Start ADC conversion
	ADC_Start(ADC0, adcStartSingle);

	// Wait for conversion to be complete
	while(!(ADC0->STATUS & _ADC_STATUS_SINGLEDV_MASK));

	// Get ADC result
	sample = ADC_DataSingleGet(ADC0);

	// Calculate input voltage in mV
	temp = (sample * 2500) / 4096;

	return temp;
}
