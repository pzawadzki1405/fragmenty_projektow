#include <avr/io.h>


// Korzystanie z przetwornika sprowadza sie do trzech krokow:
// K1 - konfiguracja ADMUX, ADCSRA
// K2 - uruchomienie konwersji poprzez ustawienie bitu ADSC
// K3 - odczytanie wyniku - jest mo¿liwe po wyzerowaniu ADSC, wtedy wynik jest w ADCl i ADCH


void ADC_Init(void)
{

	ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0); // str. 199
	ADMUX = (1 << REFS0);    // REFS0 - wybor napiecia odniesienia AVcc
							 // ADLAR = 0 - wyrownanie wyniku konwersji do prawej (rej. ADCH|ADCL) - str. 201
}

// ADCSRA - wlaczenie przetwornika, wybor preskalera
// ADMUX - wybor napiecia odniesienia oraz kanalu, z ktorego dokonywany jest pomiar

// Bity:
// ADEN - wlaczenie przetwornika
// ADPS1, ADPS0 - preskaler 8, przy 1MHz mamy 125 kHz

// Uwaga !
// W przypadku 10-bitowej rozdzielczosci preskaler powinien byc tak dobrany, aby czestotliwosc probkowania
// miescila sie w zakresie 50kHz ... 200kHz.


int getADC(char channel)
{


	ADCL = 0;							// wynik przetwarzania jest odczytywany
	ADCH = 0;							// z tych rejestrow

	ADMUX &=~ (1<<MUX0);			    // MUX0..3 wykorzystywane dla ADC0..ADC5
	ADMUX &=~ (1<<MUX1);
	ADMUX &=~ (1<<MUX2);
	ADMUX &=~ (1<<MUX3);

    channel = channel & 0x7;		    // 3 mlodsze bity

// np. 00000001 = 00000001 & 00000111 -> 00000001
//     00000101 = 00000101 & 00000111 -> 00000101


	ADMUX = (ADMUX & 0xF8) | channel;   // ustaw kanal przetwornika


// & - bitowa koniunkcja - bit wyjsciowy jest rowny 1 jezeli dwa bity na tych samych pozycja sa rowne 1
// | - bitowa alternatywa - bit wyjsciowy jest rowny 1 gdy przynajmniej jeden jest rowny 1


// channel = 1 -> 00000001,  channel = 5 -> 00000101
// 0xF8 = 11111000
// ADMUX = 01000000  z ADMUX = (1 << REFS0);

// (01000000 & 11111000) = 01000000 // nawias ze wzgledu na nizszy priorytet &
// 01000000 | 00000001 = 01000001



   ADCSRA |= (1<<ADSC);         // rozpoczêcie pomiaru
   while(ADCSRA & (1<< ADSC));	// sprawdzanie bitu ADSC, po zakonczeniu pomiaru, jest on automatycznie zerowany
   return ADCW;
}



