#ifndef _HELPER_FUNCTIONS_H_
#define _HELPER_FUNCTIONS_H_
#include "stm32f407xx.h"

void FPU_init(void);
void set_gpio(GPIO_TypeDef* gpio,int pin);
void reset_gpio(GPIO_TypeDef* gpio,int pin);
void delay_without_interrupt(int msec);
int read_gpio(GPIO_TypeDef* gpio,int pin);
void set_push(GPIO_TypeDef* gpio,int pin);
void reset_push(GPIO_TypeDef* gpio,int pin);
int init_led(GPIO_TypeDef* gpio,int pin);
void led_blink(GPIO_TypeDef* gpio,int pin,int times);
void toggle_output(GPIO_TypeDef* gpio,int pin);
int check_operation(int num);

#endif

