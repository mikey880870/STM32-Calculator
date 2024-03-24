#include "7seg.h"
#include "stm32f407xx.h"
#include "helper_functions.h"

int init_7seg(GPIO_TypeDef* gpio,int DIN,int CS,int CLK){
	if(gpio==GPIOA){
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	}
	else{
		return -1;
	}
	//output mode
	gpio->MODER &= ~(0b11 << (2*DIN));
	gpio->MODER |= (0b01 << (2*DIN));
	gpio->MODER &= ~(0b11 << (2*CS));
	gpio->MODER |= (0b01 << (2*CS));
	gpio->MODER &= ~(0b11 << (2*CLK));
	gpio->MODER |= (0b01 << (2*CLK));
	
	send_7seg(gpio,DIN,CS,CLK,SEG_ADDRESS_DISPLAY_TEST,0x01); //DISPLAY TEST OPEN
	delay_without_interrupt(1000);
	send_7seg(gpio,DIN,CS,CLK,SEG_ADDRESS_DISPLAY_TEST,0x00); //DISPLAY TEST CLOSE
	delay_without_interrupt(1000);
	send_7seg(gpio,DIN,CS,CLK,SEG_ADDRESS_DISPLAY_TEST,0x01); 
	delay_without_interrupt(1000);
	send_7seg(gpio,DIN,CS,CLK,SEG_ADDRESS_DISPLAY_TEST,0x00); 
	delay_without_interrupt(1000);
	send_7seg(gpio,DIN,CS,CLK,SEG_ADDRESS_DISPLAY_TEST,0x01); 
	delay_without_interrupt(1000);
	send_7seg(gpio,DIN,CS,CLK,SEG_ADDRESS_DISPLAY_TEST,0x00); 
	return 0;
}

void send_7seg(GPIO_TypeDef* gpio,int DIN,int CS,int CLK,int address,int data){
	int payload = ((address & 0xFF)<<8)|(data&0xFF); //command (D8~D15),data (D0~D7)
	int total_cylces = 16+1;
	for(int i = 1;i <= total_cylces;i++){
		reset_gpio(gpio,CLK);    //clock off
		
		if(((payload>>(total_cylces-1-i))&0x1) && i!=total_cylces){
			set_gpio(gpio,DIN);
		}
		else{
			reset_gpio(gpio,DIN);
		}
		
		if(i == total_cylces){
			set_gpio(gpio,CS);
		}
		else{
			reset_gpio(gpio,CS);
		}
		set_gpio(gpio,CLK);
	}
	return;
}
		
int display_num(GPIO_TypeDef* gpio,int DIN,int CS,int CLK,int num,int num_digs){
	for(int i = 1;i <= num_digs;i++){
		send_7seg(gpio,DIN,CS,CLK,i,num%10);
		num /= 10;
	}
	for(int i = num_digs+1;i <= 8;i++){
		num /= 10;
		send_7seg(gpio,DIN,CS,CLK,i,15);
	}
	if(num != 0){
		return -1;
	}
	return 0;
}

int num_digits(int x){
	int res = 0;
	if(x == 0){
		return 1;
	}
	while(x){
		res++;
		x /= 10;
	}
	return res;
}
		

