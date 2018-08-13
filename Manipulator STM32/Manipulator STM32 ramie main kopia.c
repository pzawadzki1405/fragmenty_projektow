/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f10x.h"
#include "system.h"
#include "usart.h"
#include <stdio.h>
#include "spi.h"
#define POM 30


float x_a[POM], y_a[POM], z_a[POM];
float x_af[POM], y_af[POM], z_af[POM];
int a_st[POM];
int a_st_zero=0, a_st_spr=0, a_st_zeron=0, a_st_sum=0;
float dt=0.01;

int chwytak = 0;


void EXTI15_10_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line13)) {		//przycisk 1
	 if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0) { // jesli przycisk jest przycisniety
		 chwytak = chwytak+1;
		 if (chwytak>1000) chwytak = 1000;
	 }
	 EXTI_ClearITPendingBit(EXTI_Line13);
	}

	if (EXTI_GetITStatus(EXTI_Line14)) {		//przycisk 2
	 if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0) { // jesli przycisk jest przycisniety
		 chwytak = chwytak-1;
		 if (chwytak<=1) chwytak = 1;
	 }
	 EXTI_ClearITPendingBit(EXTI_Line14);
	}

}


int main(void)
{
	System_Init();
	delay_ms(1000);
	USART_PARAM();
	SPI_PARAM();


	if (Czujnik_Init(NADGARSTEK) == SPI_ERROR){

		while(1){printf("blad inicjalizacji czujnika nadgarstka \n");}
	}
	if (Czujnik_Init(PRZEDRAMIE) == SPI_ERROR){

		while(1){printf("blad inicjalizacji czujnika nadgarstka \n");}
	}
	if (Czujnik_Init(RAMIE) == SPI_ERROR){

		while(1){printf("blad inicjalizacji czujnika nadgarstka \n");}
	}

	filtr_param();
	acc_wyzeruj();


	for(;;){



	}
}
