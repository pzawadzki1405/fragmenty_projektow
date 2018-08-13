#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "funkcje.h"
#include "profiler.h"
#include "funkcje/usart.h"
#include "funkcje/adc.h"
#include "funkcje/zyroskop.h"
#include "rezerwowy.h"

//------------------ZMIENNE Z PRZERWAN -----------------------//
//zmienne wykorzystane w przerwaniach, volatile oznacza zmienna ulotna-niezbedne do poprawnej pracy w przerwaniach
volatile int j=0;	//zmienna do zliczania impulsow z enkoder lewy
volatile int p=0;	//zmienna do zliczania impulsow z enkoder prawy
volatile int cmj=0;	//zmienna pomocnicza (enk lewy) sluzaca do zapisu co 94 impulsy (94 impulsy to okolo 1cm), PWM100/256= ~18,73 cm/s
volatile int cmp=0;	//zmienna pomocnicza (enk prawy) sluzaca do zapisu co 94 impulsy (94 impulsy to okolo 1cm)
volatile int interupt=0;//zmienna pomocnicza do przerwania wewnetrznego
volatile int ms=0;		//zmienna pomocnicza do zliczania co 5 przerwania wewnetrznego
uint8_t timer_start = 185;//zmienna do ustalenia poczatkowej wartosci timera, do precyzyjnego odliczania czasu


//------------------ZMIENNE DO PWM -----------------------//
//uint8_t oznacza zmienna unsigned(bez znaku) 8-bitowa
int aktPWM_L=150;	//zmienna pomocnicza sluzaca do zapisu zmiany PWM_L
int aktPWM_P=150;	//zmienna pomocnicza sluzaca do zapisu zmiany PWM_P
uint8_t maxPWM = 20;	//zmienna pomocnicza sluzaca do ustalenia maksymalnego PWM
//definicje do PWM
#define  pwm_p OCR1A
#define  pwm_l OCR1B

//------------------ZMIENNE Z PROFILERA -----------------------//
//uint16_t oznacza zmienna bez znaku 16-bitowa
uint16_t oblicz_S=0;		//obliczana wartosc drogi, do podania w regulatorze
uint16_t przysp_8;		//przyspieszenie, sluzy do zwiekszania predkosci
uint16_t targetS;		//droga zadana, docelowa
//int16_t oznacza zmienna ze znakiem 16-bitowa
int16_t docelowaV_8=0;	//docelowa - predkosc ustalona, do ktorej dazy profiler
int16_t aktualnaV_8=0; 	//aktualna - predkosc ktora bedzie zwiekszana lub zmniejszana
int16_t nextV_8;	 	//next - predkosc w nastepnym ruchu
int uchybS_pop=0, uchybR_pop=0;	//poprzedni uchyb translacji oraz poprzedni uchyb rotacji

//definicje zmiany stanow, do modulu sterujacego ruchem myszy
int stan; //zmienna stanu
int stan2;
int stan3;

#define KONIEC_SKRET 8
#define SKRET_L 7
#define PODJEDZ 6
#define PROSTO 5
#define OBROT 4
#define SKRET 3

#define JAZDA 2
#define HAMOWANIE 1
#define KONIEC 0

int doprzodu=1, obroc=1;

//------------------ZMIENNE Z ZYROSKOPU -----------------------//
int16_t x_gyro, y_gyro, z_gyro;


//zmiene do profilra rotacji
int16_t oblicz_R=0;		//obliczana wartosc kata, do podania w regulatorze
int16_t targetR=0;		//k¹t zadany, docelowy
int uchybS_U;

int c_ls, c_ps;						//zmienne z czujnikow
int Kpc, Kpd, Tdc, Tdd;		//zmienne do regulatora
float Kp, Td;
//------------------FUNKCJE PRZERWAN -----------------------//
ISR(INT0_vect){		//przerwanie zew INT0 (enk lewy)
	 if (stan == JAZDA || stan == HAMOWANIE)
	 {

			j=j+1;		//zliczanie przerwan

	 }


	//if (j>=94){ j=0; cmj=cmj+1; }	//co 94 impulsy (okolo 1cm) zwieksz cmj
}

ISR(INT1_vect){		//przerwanie zew INT1 (enk prawy)
	 if (stan == JAZDA || stan == HAMOWANIE)
	 {
			p=p+1;		//zliczanie przerwan

	 }
	//if (p>=94){ p=0; cmp=cmp+1; }	//co 94 impulsy (okolo 1cm) zwieksz cmp
}

ISR(TIMER0_OVF_vect){	//przerwanie od przepelnienia timer1 (przerwanie wewnetrzne do odliczania czasu)
	TCNT0 = timer_start;	//wartosc poczatkowa timera, do precyzyjnego ustalenia czasu
/*	if (stan == JAZDA || stan == HAMOWANIE) {
		PORTB |= (1<<PB0);
		jazda();		//funkcja do jazdy (z profilera)
		regulatorPD(Kp, Td, 80, 80);
		PORTB &= ~(1<<PB0);
	}*/
	if (c_ps>45 && c_ls>45) regulator_rez(Kp, Td, 45, 45);
	if (c_ps<45 && c_ls<45){
		regulator_rez(Kp, Td, 55, 52); _delay_ms(2);
	}
	if (c_ps<45) {_delay_ms(7);regulator_rez(Kp, Td, 55, 52);}
	if (c_ls<45){ _delay_ms(7);regulator_rez(Kp, Td, 52, 55);}
	/*else {
		j=p=0;
		regulator_rez2(0.005, 0.003, 50, 50);
	}*/
}


//------------------FUNKCJA MAIN---------------------------//
int main(void)
{
	_delay_ms(2000);		//opoznienie wlaczenia

	//--------Deklaracja ustawien ---------//
	porty();
	USART_Init();
	ADC_Init();
	pwm();
	sei();	//globalne w³¹czenie przerwañ
	przerwania();


/*	//------Wartosci poczatkowe do jazdy ----//
	oba_przod();
	aktualnaV_8 = 0<<8;
	nextV_8=0;
	stan=JAZDA;
	stan2=PROSTO;
	param_prosto();*/

	//----Wartosci wzmocnienia do regulatora----//

/*	Kp=0.05;
	Td=0.08;*/
	Kp=0.09;
	Td=0.9;
	stan=JAZDA;
	//----------Petla while -----------//
	while(1){
		pomiary();
		uart_wykresy();


/*		if(czujnik_ps()<=75){
					stan=KONIEC;
					oba_stop();
					_delay_ms(20);

					oba_przod();
					_delay_ms(2000);

					oba_stop();
					_delay_ms(500);

					oba_prawo();
					_delay_ms(1000);

					oba_lewo();
					_delay_ms(15);

					oba_przod();
					_delay_ms(700);

					stan=JAZDA;
		}else{*/
			oba_przod();



 		pwm_l=(aktPWM_L);	//podpisanie pwm
 		pwm_p=(aktPWM_P);

	}//koniec while
}//koniec main
