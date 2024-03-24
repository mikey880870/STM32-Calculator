#include "stm32f407xx.h"
#include "helper_functions.h"
#include "timer.h"
#include "keypad.h"
#include "7seg.h"
#include "stdbool.h"
#include "stdio.h"

#define SEG_gpio GPIOA
#define DIN_pin 3
#define CS_pin 4
#define CLK_pin 5

#define COL_gpio GPIOB
#define COL_pin 11   //PB14 -> 1

#define ROW_gpio GPIOC 
#define Row_pin 6

#define LED_gpio GPIOD
#define LED_pin 15

#define COUNTER_timer TIM3

int now_col = 3;
double num1 = 0,num2 = 0;
int op = 0;
int keycnt = 0,keyvalue = -1;

int keypad[4][4] = {  // 11:+,12:-,13:*,14:clear,15:/
	{1,2,3,11},
	{4,5,6,12},
	{7,8,9,13},
	{16,0,15,14}
	};

void EXTIkeypadHanlder(int r){
	int nowkey = keypad[r][(now_col+3)%4];
	if(nowkey == keyvalue){
		keycnt++;
	}
	else{
		keycnt = 0;
	}
	keyvalue = nowkey;
	if(keycnt >= 1){
		keycnt = 0;
		
		if((nowkey < 10) && (op == 0) && (num_digits(num1)<3)){
			num1 = num1*10 + keyvalue;
			display_num(SEG_gpio,DIN_pin,CS_pin,CLK_pin,num1,num_digits(num1));
		}
		else if(nowkey < 10 && op != 0 && (num_digits(num2)<3)){
			reset_timer(COUNTER_timer);
			num2 = num2*10 + keyvalue;
			timer_start(COUNTER_timer);
			display_num(SEG_gpio,DIN_pin,CS_pin,CLK_pin,num2,num_digits(num2));
		}
		else if(nowkey > 10){
			op = keyvalue;
			if(op == 15){
				num1 = 0;
				num2 = 0;
			}
			display_num(SEG_gpio,DIN_pin,CS_pin,CLK_pin,0,0);
		}
	}
}

void TIM3_IRQHandler(void){
	if(TIM3->SR & TIM_SR_UIF){
		double result = 0;
		switch(op){
			case 11:
				result = num1 + num2;
				break;
			case 12:
				result = num1 - num2;
				break;
			case 13:
				result = num1*num2;
				break;
			case 14:
				result = num1/num2;
				break;
		}
		display_num(SEG_gpio,DIN_pin,CS_pin,CLK_pin,result,num_digits(result));
		timer_stop(COUNTER_timer);
		num1 = 0;
		num2 = 0;
		op = 0;
		TIM3->SR &= ~TIM_SR_UIF; //Clean UIF Flag
	}
}

void SysTick_Handler(){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		reset_push(COL_gpio,now_col+COL_pin);
		now_col = (now_col+1)%4;
		set_push(COL_gpio,now_col+COL_pin);
	}
}

void EXTI9_5_IRQHandler(){
	if(EXTI->PR & EXTI_PR_PR6_Msk){
		EXTIkeypadHanlder(0);
		EXTI->PR = EXTI_PR_PR6_Msk;
	}
	if(EXTI->PR & EXTI_PR_PR7_Msk){
		EXTIkeypadHanlder(1);
		EXTI->PR = EXTI_PR_PR7_Msk;
	}
	if(EXTI->PR & EXTI_PR_PR8_Msk){
		EXTIkeypadHanlder(2);
		EXTI->PR = EXTI_PR_PR8_Msk;
	}
	if(EXTI->PR & EXTI_PR_PR9_Msk){
		EXTIkeypadHanlder(3);
		EXTI->PR = EXTI_PR_PR9_Msk;
	}
}

void EXTI_Setup(){
	//Enable SYSCFG CLk
	RCC->APB2ENR = RCC_APB2ENR_SYSCFGEN;
	//select output bits
	SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI6_Msk;
	SYSCFG->EXTICR[1] |= (0b0010 << SYSCFG_EXTICR2_EXTI6_Pos);
	SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI7_Msk;
	SYSCFG->EXTICR[1] |= (0b0010 << SYSCFG_EXTICR2_EXTI7_Pos);
	SYSCFG->EXTICR[2] &= ~SYSCFG_EXTICR3_EXTI8_Msk;
	SYSCFG->EXTICR[2] |= (0b0010 << SYSCFG_EXTICR3_EXTI8_Pos);
	SYSCFG->EXTICR[2] &= ~SYSCFG_EXTICR3_EXTI9_Msk;
	SYSCFG->EXTICR[2] |= (0b0010 << SYSCFG_EXTICR3_EXTI9_Pos);
	//Enable interrupt
	EXTI->IMR |= EXTI_IMR_IM6;
	EXTI->IMR |= EXTI_IMR_IM7;
	EXTI->IMR |= EXTI_IMR_IM8;
	EXTI->IMR |= EXTI_IMR_IM9;
	// Enable falling edge
	EXTI->FTSR |= EXTI_FTSR_TR6;
	EXTI->FTSR |= EXTI_FTSR_TR7;
	EXTI->FTSR |= EXTI_FTSR_TR8;
	EXTI->FTSR |= EXTI_FTSR_TR9;
	// Enable NVIC
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

int main(void){
	FPU_init();

	if(init_7seg(SEG_gpio,DIN_pin,CS_pin,CLK_pin)!=0){
		return -1;
	}
	
	if(init_keypad(ROW_gpio,COL_gpio,Row_pin,COL_pin)!=0){
		return -1;
	}
	
	if(init_led(LED_gpio,LED_pin)!=0){
		return -1;
	}
	
	send_7seg(SEG_gpio,DIN_pin,CS_pin,CLK_pin,SEG_ADDRESS_DECODE_MODE,0xFF);
	send_7seg(SEG_gpio,DIN_pin,CS_pin,CLK_pin,SEG_ADDRESS_SCAN_LIMIT,0x07);
	send_7seg(SEG_gpio,DIN_pin,CS_pin,CLK_pin,SEG_ADDRESS_SHUTDOWN,0x01);
	
	System_Config_Interrupt(10,1000);    
	EXTI_Setup();
	

	timer_enable(COUNTER_timer);
	timer_init(COUNTER_timer,10000,5000);     // Counter =  Fclk / (psc-1)(Arr-1)  (psc->16bit) 5sec

	while(1){
	}
	
	return 0;
}
	