#ifndef __TIMERDRV_H_
#define __TIMERDRV_H_

void Timer_init(void);
void setupTimer0(void);
void setupTimer1(void);
void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);
void delayms(uint32_t ms);

#endif /* INLCUDE_TIMER_H_ */
