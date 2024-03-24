#ifndef _TIMER_H_
#define _TIMER_H_
#include "stm32f407xx.h"

void System_Config_Interrupt(int speed,int load);
void SystemClock_Config(int speed);

void timer_enable(TIM_TypeDef* timer);
void timer_disable(TIM_TypeDef* timer);
void timer_reload(TIM_TypeDef* timer);
void timer_init(TIM_TypeDef* timer,int psc,int arr);
void timer_start(TIM_TypeDef* timer);
void timer_stop(TIM_TypeDef* timer);
void reset_timer(TIM_TypeDef* timer);
void timer_interrupt_init(TIM_TypeDef* timer);

#endif
