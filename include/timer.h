#ifndef __TIMERDRV_H_
#define __TIMERDRV_H_

#define MAX_TICK (0xfffffc17)

volatile uint32_t g_Ticks;
volatile uint32_t g_timerout_Ticks;// tick time = 10ms.
volatile uint32_t g_S_WGog_Ticks;// tick time = 10ms.
volatile uint32_t g_S_WGog_SW; //switcher

void Timer_init(void);
void setupTimer0(void);
void setupTimer1(void);
void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);
void delayms(uint32_t ms);

#endif /* INLCUDE_TIMER_H_ */
