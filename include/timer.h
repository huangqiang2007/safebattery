#ifndef __TIMERDRV_H_
#define __TIMERDRV_H_

#define MAX_TICK (0xfffffc17)

volatile uint32_t g_Ticks;

volatile uint32_t g_timerout_Ticks;// tick time = 10ms.


//#define g_S_WDog_Ticks_TEN 10
//#define g_S_WDog_Ticks_1Hundred 100
//#define g_S_WDog_Ticks_2Hundred 200
//#define g_S_WDog_Ticks_1Thousand 1000
//
////volatile uint32_t g_S_WDog_Ticks;// tick time = 10ms.
//
//
///*
// * define software watch dog switcher's states
// * */
//typedef enum {
//	g_S_WDog_SW_DISABLE  = 0,
//	g_S_WDog_SW_ENABLE = 1
//} g_S_WDog_SW_TypeDef;
//volatile g_S_WDog_SW_TypeDef g_S_WDog_SW; //switcher


void Timer_init(void);
void setupTimer0(void);
//void setupTimer1(void);
//void Delay_ms(uint32_t ms);
//void Delay_us(uint32_t us);
//void delayms(uint32_t ms);

#endif /* INLCUDE_TIMER_H_ */
