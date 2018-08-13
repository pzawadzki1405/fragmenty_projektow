/*
 * l3gd20h.c
 *
 *  Created on: 20 lis 2016
 *      Author: Przemek
 */

#include "spi.h"
#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>
#include "system.h"
#include <math.h>
#define POM 30

extern float x_a[POM], y_a[POM], z_a[POM];
extern float x_af[POM], y_af[POM], z_af[POM];
extern int a_st[POM];
extern int a_st_zero, a_st_spr, a_st_zeron, a_st_sum;
extern float dt;

void SPI_Transmit(char cData)
{
    SPI_I2S_SendData(SPI1, cData);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    SPI_I2S_ReceiveData(SPI1);
}


char SPI_Receive(void)
{
    SPI_I2S_SendData(SPI1, 0xFF);

    //czekaj na ramke
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    return SPI_I2S_ReceiveData(SPI1);
}


void SPI_Enable(char czujnik){
	switch(czujnik){
		case NADGARSTEK:
		GPIO_ResetBits(GPIO_NADGARSTEK, GPIO_Pin_NADGARSTEK);
		break;

		case PRZEDRAMIE:
		GPIO_ResetBits(GPIO_PRZEDRAMIE, GPIO_Pin_PRZEDRAMIE);
		break;

		case RAMIE:
		GPIO_ResetBits(GPIO_RAMIE, GPIO_Pin_RAMIE);
		break;

		default:
		//tu mozna wpisac komunikat bledu
		break;

	}
}

void SPI_Disable(char czujnik){
	switch(czujnik){
		case NADGARSTEK:
		GPIO_SetBits(GPIO_NADGARSTEK, GPIO_Pin_NADGARSTEK);
		break;

		case PRZEDRAMIE:
		GPIO_SetBits(GPIO_PRZEDRAMIE, GPIO_Pin_PRZEDRAMIE);
		break;

		case RAMIE:
		GPIO_SetBits(GPIO_RAMIE, GPIO_Pin_RAMIE);
		break;

		default:
		//tu mozna wpisac komunikat bledu
		break;

	}
}

void SPI_PARAM(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	//zegar SPI1
	GPIO_InitTypeDef gpio;
  	// Configure SPI1 pins: SCK, MISO and MOSI
  	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  	gpio.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &gpio);

  	// Configure I/O for Flash Chip select
  	gpio.GPIO_Pin = GPIO_Pin_NADGARSTEK;
  	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIO_NADGARSTEK, &gpio);

  	gpio.GPIO_Pin = GPIO_Pin_PRZEDRAMIE;
  	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIO_PRZEDRAMIE, &gpio);

  	gpio.GPIO_Pin = GPIO_Pin_RAMIE;
  	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIO_RAMIE, &gpio);

	SPI_InitTypeDef spi;
	SPI_StructInit(&spi);
 	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//mozliwosc jednoczesnego nadawania i odbierania w tym samym momencie
  	spi.SPI_Mode = SPI_Mode_Master;							//urzadzenie master
  	spi.SPI_DataSize = SPI_DataSize_8b;						// 8-bitowa ramka danych
  	spi.SPI_CPOL = SPI_CPOL_High;							// stan wysoki lini zegarowej, gdy nie sa przesylane dane
  	spi.SPI_CPHA = SPI_CPHA_2Edge;							// okresla ktorym zboczem ma byc aktywowany zegar
  	spi.SPI_NSS = SPI_NSS_Soft;								// programowa obsluga pinu CS
  	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	// preslaker zegara
  	spi.SPI_FirstBit = SPI_FirstBit_MSB;					// kolejnosc przesylania butow
  	spi.SPI_CRCPolynomial = 7;								// system kontroli poprawnosci danych
  	SPI_Init(SPI1, &spi);

  	// Enable SPI1
  	SPI_Cmd(SPI1, ENABLE);
}

/************************* SPI Functions - end *****************************************/
//a
char Czujnik_Init(char czujnik){


	char ok=0, ustawienia, wart=0;

	//printf("xa %d \n", wart);
	delay_ms(20);

	//sprawdzenie rejestru WHO_AM_I
	Gyro_Read_Register(WHO_AM_I, &wart, czujnik);
	if (wart == 0x69) ok++;

	//printf("xb %d \n", wart);
	delay_ms(20);

	//wpisanie oraz sprawdzenie rejestru CTRL1_XL
	ustawienia = ODR_XL | FS_XL;
	Gyro_Write_Register(CTRL1_XL, ustawienia, czujnik);	//wpisz ustawienia zyroskopu
	Gyro_Read_Register(CTRL1_XL, &wart, czujnik);			//odczytaj ustawienia zyroskopu
	if (wart == ustawienia) ok++;

	//printf("xc %d \n", wart);
	delay_ms(20);

	//wpisanie oraz sprawdzenie rejestru CTRL2_G
	ustawienia = ODR_G | FS_G | FS_125;
	Gyro_Write_Register(CTRL2_G, ustawienia, czujnik);	//wpisz ustawienia zyroskopu
	Gyro_Read_Register(CTRL2_G, &wart, czujnik);			//odczytaj ustawienia zyroskopu
	if (wart == ustawienia) ok++;

	//printf("xd %d \n", wart);
	delay_ms(20);

	//wpisanie oraz sprawdzenie rejestru CTRL3_C
	ustawienia = IF_INC;
	Gyro_Write_Register(CTRL3_C, ustawienia, czujnik);	//wpisz ustawienia zyroskopu
	Gyro_Read_Register(CTRL3_C, &wart, czujnik);			//odczytaj ustawienia zyroskopu
//	if (wart == ustawienia) ok++;

	//printf("xe %d \n", ok);
	delay_ms(20);

	if (ok == ALL_OK)	return SPI_OK;	//wystaw komunikat bledu
	else return SPI_ERROR;



}

void Gyro_Write_Register(char adr, char data, char czujnik){

	SPI_Enable(czujnik);
	SPI_Transmit(SPI_WRITE | adr);				//wybierz adres CTRL1
	SPI_Transmit(data);	//wpisz do tego adresu ustawienia
	SPI_Disable(czujnik);
}

void Gyro_Read_Register(char adr, char *data, char czujnik){

	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | adr);				//wybierz adres CTRL1
	*data=SPI_Receive();
	SPI_Disable(czujnik);
}

void gyro_pobierz(int16_t *x, int16_t *y, int16_t *z, char czujnik){
	int8_t x_l, x_h, y_l, y_h, z_l, z_h;

	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTX_L_G);	//odczytaj kilka wartosci zaczynajac od x_l

	x_l = SPI_Receive();
	x_h = SPI_Receive();
	y_l = SPI_Receive();
	y_h = SPI_Receive();
	z_l = SPI_Receive();
	z_h = SPI_Receive();
	SPI_Disable(czujnik);

	*x = x_h << 8 | x_l;	//przesun o 8 bit starsza czesc x
	*y = y_h << 8 | y_l;	//przesun o 8 bit starsza czesc y
	*z = z_h << 8 | z_l;	//przesun o 8 bit starsza czesc z
}

void accelerometer_pobierz(int16_t *x, int16_t *y, int16_t *z, char czujnik){
	int8_t x_l, x_h, y_l, y_h, z_l, z_h;

	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTX_L_XL);	//odczytaj kilka wartosci zaczynajac od x_l

	x_l = SPI_Receive();
	x_h = SPI_Receive();
	y_l = SPI_Receive();
	y_h = SPI_Receive();
	z_l = SPI_Receive();
	z_h = SPI_Receive();
	SPI_Disable(czujnik);

	*x = x_h << 8 | x_l;	//przesun o 8 bit starsza czesc x
	*y = y_h << 8 | y_l;	//przesun o 8 bit starsza czesc y
	*z = z_h << 8 | z_l;	//przesun o 8 bit starsza czesc z
}

float acc_x(char czujnik){
	int8_t x_l, x_h;
	int16_t xb;
	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTX_L_XL);	//odczytaj kilka wartosci zaczynajac od x_l
	x_l = SPI_Receive();
	x_h = SPI_Receive();
	SPI_Disable(czujnik);
	xb = x_h << 8 | x_l;
	return (xb*SENS_XL_2);
}

void gyro_x(int16_t *x, char czujnik){
	int8_t x_l, x_h;
	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTX_L_G);	//odczytaj kilka wartosci zaczynajac od x_l
	x_l = SPI_Receive();
	x_h = SPI_Receive();
	SPI_Disable(czujnik);
	*x = x_h << 8 | x_l;
}

float acc_y(char czujnik){
	int8_t y_l, y_h;
	int16_t yb;
	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTY_L_XL);	//odczytaj kilka wartosci zaczynajac od x_l
	y_l = SPI_Receive();
	y_h = SPI_Receive();
	SPI_Disable(czujnik);
	yb = y_h << 8 | y_l;
	return (yb*SENS_XL_2);
}

void gyro_y(int16_t *y, char czujnik){
	int8_t y_l, y_h;
	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTY_L_G);	//odczytaj kilka wartosci zaczynajac od x_l
	y_l = SPI_Receive();
	y_h = SPI_Receive();
	SPI_Disable(czujnik);
	*y = y_h << 8 | y_l;
}

float acc_z(char czujnik){
	int8_t z_l, z_h;
	int16_t zb;
	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTZ_L_XL);	//odczytaj kilka wartosci zaczynajac od x_l
	z_l = SPI_Receive();
	z_h = SPI_Receive();
	SPI_Disable(czujnik);
	zb = z_h << 8 | z_l;
	return (zb*SENS_XL_2);
}

void gyro_z(int16_t *z, char czujnik){
	int8_t z_l, z_h;
	SPI_Enable(czujnik);
	SPI_Transmit(SPI_READ | OUTZ_L_G);	//odczytaj kilka wartosci zaczynajac od x_l
	z_l = SPI_Receive();
	z_h = SPI_Receive();
	SPI_Disable(czujnik);
	*z = z_h << 8 | z_l;
}

void gyro_stopnie(float *x, float *y, float *z, char czujnik){

	int16_t xb,yb,zb; 	//wartosci 16-bitowe
	/*delay_ms(3);
	gyro_x(&xb, czujnik);
	delay_ms(3);
	gyro_y(&yb, czujnik);
	delay_ms(3);
	gyro_z(&zb, czujnik);*/
	gyro_pobierz(&xb, &yb, &zb, czujnik);
	*x=xb*SENS_G_245;
	*y=yb*SENS_G_245;
	*z=zb*SENS_G_245;
}

void accelerometer_stopnie(float *x, float *y, float *z, char czujnik){

	int16_t xb,yb,zb; 	//wartosci 16-bitowe
/*	delay_ms(3);			//pobierz pojedynczo
	acc_x(&xb, czujnik);
	delay_ms(3);
	acc_y(&yb, czujnik);
	delay_ms(3);
	acc_z(&zb, czujnik);*/
	accelerometer_pobierz(&xb, &yb, &zb, czujnik);
	*x=xb*SENS_XL_2;
	*y=yb*SENS_XL_2;
	*z=zb*SENS_XL_2;
}


struct osie{
	float x;
	float y;
	float z;
}x_post,x_pri,v_post,v_pri;


struct parametry_filtru{
	float x;
	float y;
	float z;
}alfa,beta;

void filtr_param(){
	x_post.x=0; x_post.y=0; x_post.z=0;
	x_pri.x=0; x_pri.y=0; x_pri.z=0;
	v_post.x=0; v_post.y=0; v_post.z=0;
	v_pri.x=0; v_pri.y=0; v_pri.z=0;

	alfa.x=0.12; alfa.y=0.12; alfa.z=0.12;

	beta.x=0.008; beta.y=0.008; beta.z=0.008;
}

void acc_wyzeruj(){
	for (int i=0; i<POM; i++){
		x_a[i]=0;y_a[i]=0;z_a[i]=0;
		x_af[i]=0;y_af[i]=0;z_af[i]=0;
		a_st[i]=0;
	}
}

void acc_pobierz(char czujnik){

	for (int i=1; i<POM; i++){
			x_a[i]=acc_x(czujnik);
			if ((x_a[i]<=0.05) && (x_a[i]>=(-0.05))) x_a[i]=x_a[i-1];

			y_a[i]=acc_y(czujnik);
			if ((y_a[i]<=0.05) && (y_a[i]>=(-0.05))) y_a[i]=y_a[i-1];

			z_a[i]=acc_z(czujnik);
			if ((z_a[i]<=0.05) && (z_a[i]>=(-0.05))) z_a[i]=z_a[i-1];
			delay_ms(10);
	}

}

void alfa_beta(){
	//filtr alfa beta
	for (int i=0; i<POM; i++){

		x_pri.x = x_post.x + dt * v_post.x;
		x_pri.y = x_post.y + dt * v_post.y;
		x_pri.z = x_post.z + dt * v_post.z;
		v_pri.x = v_post.x;
		v_pri.y = v_post.y;
		v_pri.z = v_post.z;

		x_post.x = x_pri.x + alfa.x * (x_a[i] - x_pri.x);	//przemieszczenie
		x_post.y = x_pri.y + alfa.y * (y_a[i] - x_pri.y);
		x_post.z = x_pri.z + alfa.z * (z_a[i] - x_pri.z);
		v_post.x = v_pri.x + beta.x * (x_a[i] - x_pri.x) / dt;	//predkosc
		v_post.y = v_pri.y + beta.y * (y_a[i] - x_pri.y) / dt;
		v_post.z = v_pri.z + beta.z * (z_a[i] - x_pri.z) / dt;

		x_af[i]=x_post.x;		//wartosci przemieszczenia przefiltrowane
		y_af[i]=x_post.y;
		z_af[i]=x_post.z;
	}
}

void acc_oblicz_kat(){
	alfa_beta();
	for (int i=0; i<POM; i++){		//obliczenie kata
		a_st[i]=((atan2f(y_af[i], z_af[i])*31.85)+100)/2;
	}
}

void acc_pomiar_ustalzero(){
	for (int j=1; j<6; j++){	//ustalenie wartosci poczatkowej
		//a_st_sum = a_st_sum + a_st[j];
	}
	a_st_zero = a_st_sum/6;
}

void acc_pomiar_korekcja(){
	for (int i=1; i<POM; i++){
		a_st[i] = a_st[i] - a_st_zero;
		if (a_st[i]<0 && a_st_spr == 1) a_st[i] = a_st[i]+a_st_zero+a_st_zeron;
		if (a_st[i]<0 && a_st_spr == 0){
				a_st_zeron = a_st[i-1];
				a_st_spr = 1;
				a_st[i] = a_st[i]+a_st_zero+a_st_zeron;
			}
	}
}




/*void offset_gyro(float *x, float *y, float *z, char czujnik){
	float  xb,yb,zb; 	//wartosci 16-bitowe
	float  s_x=0, s_y=0, s_z=0;
	gyro_stopnie(&xb, &yb, &zb, czujnik);
	s_x = xb;
	s_y = yb;
	s_z = zb;
	for (int i=0; i<50; i++){
		gyro_stopnie(&xb, &yb, &zb, czujnik);
		s_x = s_x + xb;
		s_y = s_y + yb;
		s_z = s_z + zb;
	}
	*x = s_x/50;
	*y = s_y/50;
	*z = s_z/50;

}*/
