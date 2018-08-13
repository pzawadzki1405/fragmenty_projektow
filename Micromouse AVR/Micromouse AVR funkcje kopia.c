#include <avr/io.h>
#include <util/delay.h>
#include "funkcje.h"
#include "funkcje/usart.h"
#include "funkcje/adc.h"

extern int c_ls, c_ps;
int preskalerPWM = 3;
//1-preskaler CLK/8
//2-preskaler CLK/64
//3-preskaler CLK/256

//------------USTAWIENIA--------------//
void pwm(){		//definicje pwm
/*	8-bit
	TCCR1A |= (1<<WGM10);
	TCCR1B |= (1<<WGM12);
	TCCR1A |= (1<<COM1A1);
	TCCR1A |= (1<<COM1B1);
	TCCR1B |= (1<<CS11);
*/
	//10 bit
	TCCR1A |= (1<<WGM10);
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM12);
	TCCR1A |= (1<<COM1A1);
	TCCR1A |= (1<<COM1B1);

	if(preskalerPWM==1)	{TCCR1B |= (1<<CS11);}				//preskaler CLK/8
	if(preskalerPWM==2)	{TCCR1B |= ((1<<CS10)|(1<<CS11));}	//preskaler CLK/64
	if(preskalerPWM==3)	{TCCR1B |= (1<<CS12);}				//preskaler CLK/256
}
void porty(){		//definicje portow
	DDRD |= ((1<<PD4)|(1<<PD5)); //silnik prawy
	DDRD |= ((1<<PD6)|(1<<PD7)); //silnik lewy

	DDRB |= ((1<<PB1)|(1<<PB2)|(1<<PB0)|(1<<PB5));		//pwm
	PORTB |= (1<<PB0)|(1<<PB5);

	DDRD  &=~ ((1<<PD2)|(1<<PD3));	//enkodery (przerwania)
	PORTD |=  ((1<<PD2)|(1<<PD3));
	/*
	 porty rezerwowe
	 PB3(mosi), PB4(miso),  <- enk. kanal A
	 PB0, PB5
	 */

	//czujniki analogowe - od PC0 do PC3
	//zyroskop - PC4 i PC5
}
void przerwania(){		//definicje przerwan
	//PRZERWANIA ZEW
	MCUCR|=((1<<ISC00)|(1<<ISC01));  // wyzwalaie zboczm opadaj¹cym INT0
	MCUCR|=((1<<ISC10)|(1<<ISC11));	// wyzwalaie zboczm opadaj¹cym INT1
	GICR |= (1<<INT0) | (1<<INT1);	 // w³¹czenie przerwañ INT0 i INT1

	//PRZERWANIA WEW
	TIMSK |= (1<<TOIE0);             //Przerwanie overflow (przepe³nienie timera)
	TCCR0 |= (1<<CS02)|(1<<CS00); 	// Ÿród³em CLK, preskaler 8
	TCNT0 = 55;          			//Pocz¹tkowa wartoœæ licznika
}

//-------------Pojedyncze silniki------------//
void przod_silnikl(){
	PORTD |= (1<<4);
	PORTD &=~ (1<<5);
}
void tyl_silnikl(){
	PORTD &=~ (1<<4);
	PORTD |= (1<<5);
}
void stop_silnikl(){
	PORTD &=~ (1<<4);
	PORTD &=~ (1<<5);
}
void przod_silnikp(){
	PORTD &=~ (1<<6);
	PORTD |= (1<<7);
}
void tyl_silnikp(){
	PORTD |= (1<<6);
	PORTD &=~ (1<<7);
}
void stop_silnikp(){
	PORTD &=~ (1<<6);
	PORTD &=~ (1<<7);
}

//-------------Oba silniki------------//
void oba_przod(){
	PORTD |= ((1<<4)|(1<<7));
	PORTD &=~ ((1<<5)|(1<<6));
}
void oba_tyl(){
	PORTD &=~ ((1<<4)|(1<<7));
	PORTD |= ((1<<5)|(1<<6));
}
void oba_stop(){
	PORTD &=~ ((1<<4)|(1<<7));
	PORTD &=~ ((1<<5)|(1<<6));
}

void oba_lewo(){
		PORTD &=~ ((1<<6)|(1<<4));
		PORTD |= ((1<<7)|(1<<5));
}

void oba_prawo(){
		PORTD |= ((1<<6)|(1<<4));
		PORTD &=~ ((1<<7)|(1<<5));
}

//-------------czujniki ADC------------//
int czujnik_ps(){	//czujnik prawo gora
	return getADC(0);
}
int czujnik_ls(){	//czujnik lewo gora
	return getADC(1);
}
int czujnik_3(){
	return getADC(2);
}
int czujnik_4(){
	return getADC(3);
}

//-------------pozostale funkcje------------//
void pomiary(){
	c_ps=czujnik_ps();
	c_ls=czujnik_ls();
}








