/*
 * usart.c
 *
 *  Created on: 31 paü 2016
 *      Author: Przemek
 */

#include "stm32f10x.h"
#include <stdio.h>
#include "usart.h"
////////////
void send_char_tel(char c){
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//zabezpieczenie przed zapchaniem bufora
	USART_SendData(USART2, c);										//wysylanie danych
}

void send_char_ramie(char c){
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//zabezpieczenie przed zapchaniem bufora
	USART_SendData(USART2, c);										//wysylanie danych
}
////////////
void send_string_ramie(const char* s){
	while (*s)
		send_char_ramie(*s++);
}

void send_string_tel(const char* s){
	while (*s)
		send_char_tel(*s++);
}

//////////
char USART_Receive_tel(){
	if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
	    return USART_ReceiveData(USART2);
	} else return 0;
}

char USART_Receive_ramie(){
	if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE)) {
	    return USART_ReceiveData(USART3);
	} else return 0;
}
///////////
int __io_putchar(int c){
	if (c=='\n')
		send_char_tel('\r');
	send_char_tel(c);
	return c;
}

void USART_PARAM(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//zegar UART
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//zegar UART


    GPIO_InitTypeDef gpio;
    /////////////////////////////// tel
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_2;			//TX
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;	//funkcja alternatywna, z rezystorem podciagajacym (PUSH PULL)
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_3;				//RX
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//wejscie, bez rezystora podciagajacego (plywajace ang. floating)
    GPIO_Init(GPIOA, &gpio);
    /////////////////////////////// ramie
    gpio.GPIO_Pin = GPIO_Pin_10;			//TX
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;	//funkcja alternatywna, z rezystorem podciagajacym (PUSH PULL)
    GPIO_Init(GPIOB, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_11;				//RX
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//wejscie, bez rezystora podciagajacego (plywajace ang. floating)
    GPIO_Init(GPIOB, &gpio);


    /////////////////////////////// telefon

    USART_InitTypeDef uart,uart2;	//obiekt UART
    USART_StructInit(&uart);
    uart.USART_BaudRate = 9600;	 		//szybkosc transmisji
    USART_Init(USART2, &uart);

    USART_Cmd(USART2, ENABLE);			//uruchomienie UART

    /////////////////////////////// ramie

    USART_StructInit(&uart2);
    uart2.USART_BaudRate = 9600;	 		//szybkosc transmisji
    USART_Init(USART3, &uart2);

    USART_Cmd(USART3, ENABLE);			//uruchomienie UART

}
