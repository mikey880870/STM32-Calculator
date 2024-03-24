#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "stm32f407xx.h"

int init_keypad(GPIO_TypeDef* ROW_gpio,GPIO_TypeDef* COL_gpio,int ROW_pin,int COL_pin);
int check_keypad_input_one(GPIO_TypeDef* ROW_gpio,GPIO_TypeDef* COL_gpio,int ROW_pin,int COL_pin,int x,int y);

#endif
