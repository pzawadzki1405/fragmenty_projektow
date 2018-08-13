/*
 * system.c
 *
 *  Created on: 31 paü 2016
 *      Author: Przemek
 */


#include "stm32f10x.h"
#include "system.h"

volatile uint32_t timer_ms = 0;
void RCC_Conf(void){

	ErrorStatus HSEStartUpStatus;

	//Reset ustawien RCC
	RCC_DeInit();

	//Wlacz HSE, aby system wspolpracowal z zewnetrznym rezonatorem
	RCC_HSEConfig(RCC_HSE_ON);

	//Czekaj az HSE bedzie gotowy, zeby sygnal zegarowy byl stabilny
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	//W przypadku gdy zew rezonator nie dostarczy poprawnego sygnalu, to mikrokontroler bedzie pracowal z wewnetrznego oscylatora HSI


	if (HSEStartUpStatus == SUCCESS){
		//wlaczenie bufora dla pamieci flash
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//maksymalna czstotliwosc dla komunikacji z pamiecia flash to 24MHZ, wiec opoznienie przy czestotliwosci od 48MHz do 72MHz powinno wynosic 2 cykle
		FLASH_SetLatency(FLASH_Latency_2);

		//funkcja okresla przez jaka wartosc ma byc podzielony sygnal zegarowy dla rdzenia (HCLK), w omawianym przykladzie nie dzielimy sygnalu (dzielnik = 1)
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		//czestotliwosc pracy dla magistrali APB1 (PCLK1), maksymalna czestotliwosc wynosi 36MHz, wiec trzeba 72MHz podzielic przez 2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//maksymalna czestotliwosc dla APB2 to 72MHZ, wiec nie trzeba dzielic
		RCC_PCLK2Config(RCC_HCLK_Div1);

		//Konfiguracja petli PLL, PLLCLK=8MHz*9=72MHz, (zrodlo powielanego sygnalu, mnoznik)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

		//Uruchom PLL
		RCC_PLLCmd(ENABLE);

		//Zaczekaj az PLL poprawnie sie uruchomi
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		//ustawienie PLL jako zrodlo sygnalu zegarowego
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		//Zaczekaj az PLL bedzi sygnalem zegarowym systemu
		while(RCC_GetSYSCLKSource() != 0x08);

	}
	// wlaczenie taktowania peryferiow
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

}

void NVIC_Conf(void){
	//ustaw tablice wektorow przerwan w pamieci RAM, (adres poczatkowy pamieci, offset)
	//offset musi byc zawsze wielokrotnoscia liczby 0x100 np 0x0, 0x100, 0x200 itd
	//NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);

	//ustaw tablice wektorow w pamieci flash
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
}

void SysTick_Handler(){
	if (timer_ms) {
		timer_ms--;
	}
}

void delay_ms(int time){
	timer_ms = time;
	while (timer_ms) {};
}



void System_Init(){
	RCC_Conf();
	NVIC_Conf();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//zegar alternatywnych funkcji I/O
    SysTick_Config(SystemCoreClock / 1000);		//zegar systemowy, dzielnik=1000 to dokladnosc 1ms, milion to 1us

	//przyciski
	GPIO_InitTypeDef GPIO;			
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;
	GPIO.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO.GPIO_Mode = GPIO_Mode_IPU;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource14);

	 EXTI_StructInit(&exti);
	 exti.EXTI_Line = EXTI_Line13;
	 exti.EXTI_Mode = EXTI_Mode_Interrupt;
	 exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	 exti.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&exti);

	 EXTI_StructInit(&exti);
	 exti.EXTI_Line = EXTI_Line14;
	 exti.EXTI_Mode = EXTI_Mode_Interrupt;
	 exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	 exti.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&exti);

	 nvic.NVIC_IRQChannel = EXTI15_10_IRQn;
	 nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	 nvic.NVIC_IRQChannelSubPriority = 0x00;
	 nvic.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&nvic);


}
