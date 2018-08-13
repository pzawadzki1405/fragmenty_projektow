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


void buzzer_init(){
	//buzzer
	GPIO_InitTypeDef GPIO;
	GPIO.GPIO_Pin = GPIO_Pin_15;
	GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void buzzer(int time){
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	delay_ms(time);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);

}

void zmiana (int br, int bp, int ch, int nr, int no, int lo){

	int c=0;

	while (c == 0){
		// bark rownolegly
		if (aktbr>br){
			aktbr = aktbr - 1;
			TIM_SetCompare1(TIM1, aktbr);
		}
		if (aktbr<br){
			aktbr = aktbr + 1;
			TIM_SetCompare1(TIM1, aktbr);
		}
		// bark prostopadly
		if (aktbp>bp){
			aktbp = aktbp - 1;
			TIM_SetCompare2(TIM1, aktbp);
		}
		if (aktbp<bp){
			aktbp = aktbp + 1;
			TIM_SetCompare2(TIM1, aktbp);
		}
		// chwytak
		if (aktch>ch){
			aktch = aktch - 1;
			TIM_SetCompare1(TIM3, aktch);
		}
		if (aktch<ch){
			aktch = aktch + 1;
			TIM_SetCompare1(TIM3, aktch);
		}
		// nadgarstek ruch
		if (aktnr>nr){
			aktnr = aktnr - 1;
			TIM_SetCompare2(TIM3, aktnr);
		}
		if (aktnr<nr){
			aktnr = aktnr + 1;
			TIM_SetCompare2(TIM3, aktnr);
		}

		// nadgarstek obrot
		if (aktno>no){
			aktno = aktno - 1;
			TIM_SetCompare3(TIM3, aktno);
		}
		if (aktno<no){
			aktno = aktno + 1;
			TIM_SetCompare3(TIM3, aktno);
		}

		// lokiec
		if (aktlo>lo){
			aktlo = aktlo - 1;
			TIM_SetCompare4(TIM3, aktlo);
		}
		if (aktlo<lo){
			aktlo = aktlo + 1;
			TIM_SetCompare4(TIM3, aktlo);
		}

		delay_ms(20);

		if (aktbr == br && aktbp == bp && aktch == ch && aktnr == nr && aktno == no && aktlo == lo) c = 1;
		if (c==1) break;
	}


}
int main(void)
{
	System_Init();

	USART_PARAM();
	buzzer_init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //zegar do Timer4
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //zegar do Timer4
	GPIO_InitTypeDef gpio;
//	NVIC_InitTypeDef nvic;
	TIM_TimeBaseInitTypeDef tim;
	TIM_OCInitTypeDef  channel;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);

	TIM_TimeBaseStructInit(&tim);			//konfiguracja timera
	tim.TIM_CounterMode = TIM_CounterMode_Up;	//zliczanie w gore
	tim.TIM_Prescaler = 1440 - 1;			//preskaler (licznik zlicza od 0 wiec -1)
	tim.TIM_Period = 1000 - 1;				//okres (ile razy ma zliczac ) (max 16bit - 65535)
	TIM_TimeBaseInit(TIM1, &tim);


	TIM_TimeBaseStructInit(&tim);			//konfiguracja timera
	tim.TIM_CounterMode = TIM_CounterMode_Up;	//zliczanie w gore
	tim.TIM_Prescaler = 1440 - 1;			//preskaler (licznik zlicza od 0 wiec -1)
	tim.TIM_Period = 1000 - 1;				//okres (ile razy ma zliczac ) (max 16bit - 65535)
	TIM_TimeBaseInit(TIM3, &tim);


	//bark prostopadly
	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;	// mode PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;	//jako wyjscie
	channel.TIM_Pulse = bp;
	channel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM1, &channel);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	//bark rownolegly
	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;	// mode PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;	//jako wyjscie
	channel.TIM_Pulse = br;
	channel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM1, &channel);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);


	TIM_ARRPreloadConfig(TIM1, ENABLE);

	//TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);		//uruchomienie Timera




	//chwytak
	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;	// mode PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;	//jako wyjscie
	channel.TIM_Pulse = ch;
	channel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &channel);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//nadgarstek ruch
	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;	// mode PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;	//jako wyjscie
	channel.TIM_Pulse = nr;
	channel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM3, &channel);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//nadgarstek ruch
	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;	// mode PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;	//jako wyjscie
	channel.TIM_Pulse = no;
	channel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM3, &channel);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);



	//
	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;	// mode PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;	//jako wyjscie
	channel.TIM_Pulse = lo;
	channel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC4Init(TIM3, &channel);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);		//uruchomienie Timera


	while(1){



	}
}
