/*
 * profiler.c
 *
 *  Created on: 4 mar 2016
 *      Author: Przemek
 */
#include <avr/io.h>
#include <stdlib.h>
#include "funkcje.h"
#include "funkcje/usart.h"

//EXTERN oznacza rozszerzenie zmiennej z innego pliku (plik main.c)

extern int aktPWM_L, aktPWM_P;		//zmienne do PWM
extern volatile int j, p;				//zmienne z przerwan
extern int uchybS_pop, uchybR_pop;				//uchyb poprzedni
extern int c_ps, c_ls;
// prefix "_8" oznacza ze jest to zmienna staloprzecinkowa 8.8
extern int stan;								//zmienna stanu do profilera

#define PROSTO 5
#define OBROT 4
#define SKRET 3
#define JAZDA 2
#define HAMOWANIE 1
#define KONIEC 0

extern int16_t docelowaV_8, aktualnaV_8;		//docelowa - predkosc docelowa w tym ruchu, aktualna - aktualna predkosc
extern uint16_t przysp_8, nextV_8;				//next - predkosc w nastepym ruchu (np hamowanie po jazdzie), przysp - przyspieszenie
uint16_t enktotalS;								//calkowita droga z enkoderow (enk_L + enk_P)
extern uint16_t oblicz_S;							//droga do obliczenia (do podania w reg PD)
extern uint16_t targetS;						//docelowa droga
extern int stan2;
//zminne do profilera rotacji//
extern int16_t oblicz_R;								//zadany obrot (rotacja)     // wyliczony k¹t do przejechania - wykorzystywany jako setpoint regulatora PD
extern int16_t targetR;            			// zadany ca³kowity k¹t do przejechania



void translacja_profiler(){	//profiler przemieszczenia

	if (stan == JAZDA){
		//sprawdzenie czy trzeba hamowac
		if(((aktualnaV_8 - nextV_8)*((aktualnaV_8 + nextV_8 + 128) >> 8))/(2*przysp_8) >= targetS - oblicz_S){
			stan=HAMOWANIE;		//zmien stan profilera na hamowanie
			docelowaV_8=0;//docelowaV_8=nextV_8;		//zmien predkosc do hamowania
		}
	}

	if((stan == HAMOWANIE && aktualnaV_8 == 0)){	//sprawdzenie czy dojechal
			 stan = KONIEC;				//zmien aktualny stan na KONIEC
			docelowaV_8 = 0<<8;			//docelow\a predkosc dla stanu profilera KONIEC
	}

	if (aktualnaV_8 < docelowaV_8){					//rozpedzanie
		aktualnaV_8 += przysp_8;		//zwiekszanie predkosci
		if (aktualnaV_8>docelowaV_8) aktualnaV_8 = docelowaV_8;		//jesli przekroczono docelowa
	}

	if (aktualnaV_8 > docelowaV_8){				//zwalnianie
			aktualnaV_8 -= przysp_8;	//zmniejszanie predkosci
			if (aktualnaV_8<=docelowaV_8) aktualnaV_8 = docelowaV_8;//jesli przekroczono docelowa
	}

	oblicz_S += (aktualnaV_8 + 128) >> 8;	//zmiana zadanego przemieszczenia (+128 to zaokraglenie w gore,  128/256=0,5)

}

void regulatorPD(float Kp, float Td, int wartL, int wartP){
	int enk_trans =(j+p);			//przemieszczenie z enkoderow
	int enk_rot=0;

	if (stan2 == PROSTO) enk_rot = (p-j) +(c_ls-c_ps)*2; //rotacja z enkoderow
	else  enk_rot = (p-j)*3; //rotacja z enkoderow


	int uchyb_S = (oblicz_S)*8 - (enk_trans);	//uchyb translacji
	int uchyb_R = oblicz_R - enk_rot;	//uchyb rotacji

	int newPWM_P = (Kp*uchyb_S) + Td*(uchyb_S - uchybS_pop)		//pwm prawy, rownanie reg pd
			+ (Kp*uchyb_R) + Td*(uchyb_R - uchybR_pop);
	int newPWM_L = (Kp*uchyb_S) + Td*(uchyb_S - uchybS_pop)		//pwm lewy, rownanie reg pd
	 		- (Kp*uchyb_R) + Td*(uchyb_R - uchybR_pop);

	uchybS_pop = uchyb_S;			//uchyb poprzedni translacji
	uchybR_pop = uchyb_R;			//uchyb poprzedni rotacji



	//podpisanie nowych pwm do zmiennych
	if ((wartP+newPWM_P)<=0) aktPWM_P=0;
	else {
		if((wartP+newPWM_P)>1023) aktPWM_P=1023;
		else aktPWM_P=wartP+newPWM_P;
	}

	if ((wartL+newPWM_L)<=0) aktPWM_L=0;
	else {
		if ((wartL+newPWM_L)>1023) aktPWM_L=1023;
		else aktPWM_L=wartL+newPWM_L;
	}

}

void jazda(){		//funkcja do obslugi profilera i regulatora
	oblicz_R=0;			//zalozenie ze zadR zawsze jest zero (dopoki nie jest zrobiona funkcja rotacja_profiler)
	if (targetS != 0) translacja_profiler();
	//if (targetR != 0) rotacja_profiler();
	//regulatorPD(1.3, 0.02);		//regulator PD z zalozonymi wartoscami wzmocnienia (jeszcze nie dobrane)
}

void param_prosto(){
	oblicz_S=0; j=0; p=0;
	docelowaV_8 = (2<<8);
	przysp_8=9;
	targetS=173;
}
void param_skret(){
	oblicz_S=0; j=0; p=0;

	docelowaV_8 = (1<<8);
	docelowaV_8 = docelowaV_8 + 140;
	przysp_8=35;
	targetS=75;
}
/*void param_prosto(){
	oblicz_S=0; j=0; p=0;
	docelowaV_8 = (2<<8);
	przysp_8=95;
	targetS=53;
}
void param_skret(){
	oblicz_S=0; j=0; p=0;

	docelowaV_8 = (1<<8);
	docelowaV_8 = docelowaV_8 + 140;
	przysp_8=35;
	targetS=230;
}*/
/*
void proba_skretu(){			//zaczete, nieudane skrety
	//-----Sprawdzenie czy skret lub obrot-----//

	if (stan3 != PODJEDZ || stan2 != SKRET ){
		if(czujnik_ps()<=30){
			oba_stop();
			stan=KONIEC;
			_delay_ms(500);
			stan2=SKRET;

			stan=JAZDA;
			param_prosto();
			oba_przod();
			stan3=PODJEDZ;
		}
		if(czujnik_ls()<=30){
			oba_stop();
			stan=KONIEC;
			_delay_ms(100);
			stan2=SKRET_L;
			stan=JAZDA;
			param_skret();
			oba_lewo();
		}
		if(czujnik_ps()>=200 && czujnik_ls()>=200){
			oba_stop();
			stan=KONIEC;
			_delay_ms(500);
			stan2=OBROT;
			stan=JAZDA;
			param_skret();
			oba_prawo();
		}


	}



	if (stan == KONIEC){
		oba_stop();
		if (stan3 == PODJEDZ){
			stan3 = 0;
		}
		if (stan3 == KONIEC_SKRET){
			stan2=PROSTO;
		}
		if (stan3 == KONIEC_SKRET && stan2 == PROSTO) stan3=PODJEDZ;

		if (stan2 == PROSTO){
			_delay_ms(500);
			param_prosto();
			stan=JAZDA;
			oba_przod();
		}
		if (stan2 == SKRET){
			_delay_ms(500);

			//for (int abc=0; abc<30000; abc++){
				param_skret();
				stan=JAZDA;
				oba_prawo();
			//}
			stan3=KONIEC_SKRET;
		}
		if (stan2 == SKRET_L){
			_delay_ms(500);
			param_skret();
			stan=JAZDA;
			oba_lewo();
		}
		if (stan2 == OBROT){
			_delay_ms(500);
			param_skret();
			stan=JAZDA;
			oba_prawo();
		}


	}
}*/

/*
		if (stan == KONIEC && stan2 == PROSTO) {
			oba_stop();
			if (doprzodu <=0) {
				_delay_ms(1000);
				stan2=SKRET;
				obroc=1;
				param_skret();
				oba_prawo();
				stan=JAZDA;
			}
			if (doprzodu>0){
				_delay_ms(1000);
				oba_przod();
				param_prosto();
				stan=JAZDA;
				doprzodu=doprzodu-1;
			}

		}
		if (stan == KONIEC && stan2 == SKRET) {
			oba_stop();
			if (obroc <=0) {
				_delay_ms(1000);
				stan2=PROSTO;
				doprzodu=1;
				param_prosto();
				oba_przod();
				stan=JAZDA;
			}
			if (obroc>0){
				_delay_ms(1000);
				oba_prawo();
				param_skret();
				stan=JAZDA;
				obroc=obroc-1;
			}
		}*/
