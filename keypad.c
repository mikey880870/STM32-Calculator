#include "stm32f407xx.h"
#include "keypad.h"
#include "7seg.h"
#include "helper_functions.h"

int init_keypad(GPIO_TypeDef* ROW_gpio,GPIO_TypeDef* COL_gpio,int ROW_pin,int COL_pin){
	if(ROW_gpio == GPIOC){
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	}
	if(COL_gpio == GPIOB)
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	}
	else{
		return -1;
	}
	
	for(int i = 0;i < 4;i++){
		COL_gpio->MODER &= ~(0b11 << (2*(COL_pin+i))); 
		COL_gpio->MODER |= (0b01 << (2*(COL_pin+i)));       //output
		
		COL_gpio->OSPEEDR &= ~(0b11 << (2*(COL_pin+i)));
		COL_gpio->OSPEEDR |= (0b11 << (2*(COL_pin+i)));     // very high speed
		
		COL_gpio->OTYPER &= ~(0b1 << (COL_pin+i));
		COL_gpio->OTYPER |= (0b1 << (COL_pin+i));           // open drain (close)

		set_gpio(COL_gpio,COL_pin+i);                       // set output to high(switch open drain and push-pull)
	}
	
	for(int i = 0;i < 5;i++){
		ROW_gpio->MODER &= ~(0b11 << (2*(ROW_pin+i)));
		ROW_gpio->MODER |= (0b00 << (2*(ROW_pin+i)));       // input mode
		
		ROW_gpio->PUPDR &= ~(0b11 << (2*(ROW_pin+i)));        
		ROW_gpio->PUPDR |= (0b10 << (2*(ROW_pin+i)));       // pull-down 
	}
	return 0;
}

int check_keypad_input_one(GPIO_TypeDef* ROW_gpio,GPIO_TypeDef* COL_gpio,int ROW_pin,int COL_pin,int x,int y){ //x = row y = col
	int cycles = 400;
	
	COL_gpio->OTYPER &= ~(1 << (COL_pin+y));
	
	int cnt = 0;
	for(int i = 0;i < cycles;i++){
		cnt += read_gpio(ROW_gpio,ROW_pin+x);
	}		
	
	COL_gpio->OTYPER |= (1 << (COL_pin+y));
	return (cnt > (cycles*0.7));
}


