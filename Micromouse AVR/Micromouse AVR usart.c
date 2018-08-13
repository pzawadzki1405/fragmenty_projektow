
#include <avr/io.h>
//#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "../funkcje.h"
#include "usart.h"
#include "../profiler.h"

extern int aktPWM_L, aktPWM_P, stan;	//rozszerzenie zmiennych aktPWM z maina
extern volatile int j,p;
extern int16_t docelowaV_8, aktualnaV_8;
extern uint16_t oblicz_S;
extern int16_t oblicz_R;
extern int uchybS_U;

#define JAZDA 2
#define HAMOWANIE 1
#define KONIEC 0

void USART_Init(){		//definicja uarta
	UBRRL = 51;
	UCSRB = (1<<RXEN)|(1<<TXEN);
	//UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}


void USART_Transmit(unsigned char data){		//wyslij znak
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = data;
}

void USART_Transmit_String(char *str){			//wyslij bufor
	while (*str) USART_Transmit(*str++);
}

void USART_Transmit_tel(char z, long int number){	//przeslij na telefon, funkcja pomocnicza do skroconego zapisu
	USART_Transmit_Digit(number);
	USART_Transmit(z);
	//USART_Transmit_String("\n");
	//_delay_ms(100);
}

void USART_Transmit_Digit(long int number){	//wyslij liczbe
	char buf[5];
	USART_Transmit_String(itoa(number,buf,10)); 		//itoa - zamiana inta na ascii, itoa(liczba, bufor, typ liczby)
}


unsigned char USART_Receive(void){		//odbierz znak (z petla while)
	while ( !(UCSRA & (1<<RXC)) );		//czekaj dopoki nie pojawi sie znak na Rx
	return UDR;
}

unsigned char uart0_getc(){				//odbierz znak (bez petli while)
    return UDR;
}

void uart_wykresy(){

	if (stan == JAZDA || stan == HAMOWANIE){
		USART_Transmit_tel('a',docelowaV_8);
		USART_Transmit_tel('b',aktualnaV_8);

		USART_Transmit_tel('c',(oblicz_S)*8);
		USART_Transmit_tel('d', (j+p));

		USART_Transmit_tel('e', aktPWM_L);
		USART_Transmit_tel('f', aktPWM_P);

		USART_Transmit_tel('g', czujnik_ls());
		USART_Transmit_tel('h', czujnik_ps());

		//USART_Transmit_String("\n");
	}
// 	else{
// 		USART_Transmit_tel('a',czujnik_ps());
// 		_delay_ms(500);
// 		USART_Transmit_tel('b',czujnik_ls());
// 		_delay_ms(500);
// 	}
}

void Bluetooth(){				//funkcja obslugujaca Bluetooth w telefonie
	char key;
	key = uart0_getc();			//odbierz znak

	switch (key){
		case 'a':
		akcelerometr(key);
		break;

		case 'g':
		wyslij_licznik(key);
		break;

		case 'n':
		nastawy(key);
		break;

		case 'o':
		czujniki(key);
		break;

		default:
		break;

	}


}

void czujniki(char key){		//funkcja do menu czujniki
	while (key != 'k'){
		key = uart0_getc();		//odbierz znak
		if (key == 'k') break;	//jesli wystapi k to wyjdz

		//----TU WPISZ WARTOSCI LICZB DO WYSLANIA----//

	}

}

void nastawy(char key){			//funkcja do menu nastawy
	while (key != 'k'){
		key = uart0_getc();
		if (key == 'k') break;	//jesli wystapi k to wyjdz
		if (key == 'p'){		//zmiana pwm
			int w;
			w=receive_int(key);
			w=w*2.5;
			aktPWM_L=aktPWM_P=w;
			USART_Transmit_String("zmianaPWM ");
			USART_Transmit_Digit(w);

		}

		if (key == 'w'){		//wlasne zmienne
			while(key != 'c'){
			key = USART_Receive();
			if (key == 'c') break;
			USART_Transmit(key);
			switch (key){
				//----TU WPISZ WARTOSCI LICZB DO WYSLANIA----//
				default: break;
			}

			}
		}
	}
}

int receive_int(char key){		//odbierz inta
	unsigned char tab[4];
	int i=0;
	while(key != 'z'){
		key = USART_Receive();
		USART_Transmit(key);
		if (key == 'z') break;
		tab[i]=key;
		i=i+1;
	}
	int w=atoi(tab);		//atoi zamiana ascii na inta
	return w;
}

void wyslij_licznik(char key){
	int i=0;

	while (key != 'k'){
		key = uart0_getc();
		if (key == 'k') break;
		else{
		i=i+1;
		USART_Transmit_Digit(i);
		USART_Transmit(' ');
		_delay_ms(500);
		}
	}

}

void akcelerometr (char key){

	while (key != 'k'){
		key = USART_Receive();
		if (key == 'k') break;
		else{
				switch (key){

					case 'd':	//tyl
					oba_tyl();
					break;

					case 'g':	//tyl
					oba_przod();
					break;

					case 'l':	//tyl
					oba_lewo();
					break;

					case 'p':	//tyl
					oba_prawo();
					break;

					case 's':	//tyl
					oba_stop();
					break;

					default:
					oba_stop();
					break;
				}
		}
	}

}

