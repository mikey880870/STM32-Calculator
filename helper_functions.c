#include "stm32f407xx.h"
#include "helper_functions.h"

void FPU_init(void){
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
	__DSB();
	__ISB();
}

void set_gpio(GPIO_TypeDef* gpio,int pin){
	gpio->BSRR |= (1 << pin);
}

void reset_gpio(GPIO_TypeDef* gpio,int pin){                                                                                                                
	gpio->BSRR |= (1 << (pin + 16));
}

void delay_without_interrupt(int msec){
	int loop_cnt = 500*msec;
	while(loop_cnt){
		loop_cnt--;
	}
	return;
}

int read_gpio(GPIO_TypeDef* gpio,int pin){
	return (gpio-> IDR >> pin) & 1;       
}

void set_push(GPIO_TypeDef* gpio,int pin){
	gpio->OTYPER &= ~(1 << pin);
}

void reset_push(GPIO_TypeDef* gpio,int pin){
	gpio->OTYPER |= (1 << pin);
}

int init_led(GPIO_TypeDef* gpio,int pin){
	if(gpio == GPIOD){
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	}
	else{
		return -1;
	}
	gpio->MODER &= ~(0b11 << (2*pin));
	gpio->MODER |= (0b01 << (2*pin));
	return 0;
}

void led_blink(GPIO_TypeDef* gpio,int pin,int times){
	for(int i = 0;i < times;i++){
		set_gpio(gpio,pin);
		delay_without_interrupt(500);
		reset_gpio(gpio,pin);
		delay_without_interrupt(500);
	}
}

void toggle_output(GPIO_TypeDef* gpio,int pin){
	if(read_gpio(gpio,pin) == 0){
		set_gpio(gpio,pin);
	}
	else{
		reset_gpio(gpio,pin);
	}
}


