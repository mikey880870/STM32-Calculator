#include "timer.h"
#include "stm32f407xx.h"

void System_Config_Interrupt(int speed,int load){
	SystemClock_Config(speed);
	SysTick->LOAD = load;
	SysTick->CTRL |= (1 << SysTick_CTRL_CLKSOURCE_Pos);
	SysTick->CTRL |= (1 << SysTick_CTRL_TICKINT_Pos);
	SysTick->CTRL |= (1 << SysTick_CTRL_ENABLE_Pos);
}

void SystemClock_Config(int speed){
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;  
	RCC->CFGR |= RCC_CFGR_SW_HSI;  //16Mhz
	
	while((((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos) != 0));   // 
	
	RCC->CR &= ~RCC_CR_PLLON;  // disable PLL
	while((RCC->CR & RCC_CR_PLLRDY) != 0); 
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
	
	int set_N = 0;
	int set_M = 0;     // f(vout) = f(clock input) * (PLLN / (PLLM * PLLP))
	int set_P = 0;
	
	if(speed == 10){
		set_M = 0b101000;
		set_N = 0b110010;
		set_P = 0b00;
	} 
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	RCC->PLLCFGR |= (set_N << RCC_PLLCFGR_PLLN_Pos);
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
	RCC->PLLCFGR |= (set_M << RCC_PLLCFGR_PLLM_Pos);
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
	RCC->PLLCFGR |= (set_P <<RCC_PLLCFGR_PLLP_Pos);
	
	while ((RCC->CR & RCC_CR_PLLRDY) != 0);
	
	//enable PLL
	RCC->CR |= RCC_CR_PLLON;
	
	while ((RCC->CR & RCC_CR_PLLRDY) != 0);
	
	// system clock -> PLL
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	while((((RCC->CFGR & RCC_CFGR_SWS_Msk)>>RCC_CFGR_SWS_Pos) != 2));
}

void timer_enable(TIM_TypeDef* timer){
	if(timer == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	} 
}

void timer_disable(TIM_TypeDef* timer){
	if(timer == TIM3){
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
	}
}

void timer_reload(TIM_TypeDef* timer){
	timer->EGR |= TIM_EGR_UG;
}

void timer_init(TIM_TypeDef* timer,int psc,int arr){
	timer->CR1 &= ~TIM_CR1_DIR;   // up counting
	timer->PSC = (uint16_t)(psc-1);
	timer->ARR = (uint16_t)(arr-1);
	timer->EGR |= TIM_EGR_UG;
	//timer->CR1 |= TIM_CR1_URS;
	reset_timer(timer);
}

void timer_start(TIM_TypeDef* timer){
	timer->DIER |= TIM_DIER_UIE;
	timer->SR &= ~TIM_SR_UIF; //Clean UIF Flag
	NVIC_EnableIRQ(TIM3_IRQn);
	timer->CR1 |= TIM_CR1_CEN;
}

void timer_stop(TIM_TypeDef* timer){
	timer->CR1 &= ~TIM_CR1_CEN;
}

void reset_timer(TIM_TypeDef* timer){
	timer->CNT = 0;
}

