/*
 * l3gd20.c
 *
 *  Created on: 28 lut 2016
 *      Author: Przemek
 */


#include <avr/io.h>
#include "zyroskop.h"
#define USART_BUF_SIZE           12
uint8_t gxl, gxh, gyl, gyh, gzl, gzh;



extern int16_t x_gyro, y_gyro, z_gyro;


struct osie{
	int16_t x;
	int16_t y;
	int16_t z;
}xpost,xpri,vpost,vpri;


struct parametry_filtru{
	float x;
	float y;
	float z;
}alfa,beta;



void filtr_param(){
	xpost.x=0; xpost.y=0; xpost.z=0;
	xpri.x=0; xpri.y=0; xpri.z=0;
	vpost.x=0; vpost.y=0; vpost.z=0;
	vpri.x=0; vpri.y=0; vpri.z=0;

	alfa.x=0.5;
	alfa.y=0.5;
	alfa.z=0.5;

	beta.x=0.05;
	beta.y=0.05;
	beta.z=0.05;

}

void I2C_start(void){
     TWCR = (1<<TWINT) | (1<<TWEN) |( 1<<TWSTA);
     while (! (TWCR & (1<<TWINT)));
}

void I2C_stop(void){
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    while (! (TWCR & (1<<TWSTO)));
}

void I2C_write(uint8_t dane){
     TWDR = dane;
     TWCR = (1<<TWINT) | (1<<TWEN);
     while (! (TWCR & (1<<TWINT)));
}

uint8_t I2C_read(uint8_t ACK){
    TWCR = (1<<TWINT) | (ACK<<TWEA) | (1<<TWEN);
    while (! (TWCR & (1<<TWINT)));
    return TWDR;
}

// konwersja liczby BCD na dziesiêtn¹
uint8_t bcd2dec(uint8_t bcd) {
    return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}


void gyro_pobierz(void){
	I2C_start();
	I2C_write(0xD6);	//write gyr
	I2C_write(0x28 | (1<<7));
	I2C_start();
	I2C_write(0xD7);
	gxl=I2C_read(1);
	gxh=I2C_read(1);
	gyl=I2C_read(1);
	gyh=I2C_read(1);
	gzl=I2C_read(1);
	gzh=I2C_read(0);
	I2C_stop();

	x_gyro = gxh << 8 | gxl;
	y_gyro = gyh << 8 | gyl;
	z_gyro = gzh << 8 | gzl;

	filtrab(0.1);
}

void filtrab(float dt){

	xpri.x = xpost.x + dt * vpost.x;
	xpri.y = xpost.y + dt * vpost.y;
	xpri.z = xpost.z + dt * vpost.z;

	vpri.x = vpost.x;
	vpri.y = vpost.y;
	vpri.z = vpost.z;

	xpost.x = xpri.x + alfa.x * (x_gyro - xpri.x);
	xpost.y = xpri.y + alfa.y * (y_gyro - xpri.y);
	xpost.z = xpri.z + alfa.z * (z_gyro - xpri.z);

	vpost.x = vpri.x + beta.x * (x_gyro - xpri.x) / dt;
	vpost.y = vpri.y + beta.y * (y_gyro - xpri.y) / dt;
	vpost.z = vpri.z + beta.z * (z_gyro - xpri.z) / dt;
}
void l3gd20_init(void){
	DDRC |= ((1<<PC5)|(1<<PC4));
	PORTC |= ((1<<PC5)|(1<<PC4));
	//konfiguracja
	I2C_start();
	I2C_write(0xD6);	//write gyr
	I2C_write(0x20);
	I2C_write(0b00111111);
	I2C_stop();
	filtr_param();
}
int16_t filterx_get_xpost(){
	return xpost.x;
}

int16_t filterx_get_ypost(){
	return xpost.y;
}
int16_t filterx_get_zpost(){
	return xpost.z;
}

int16_t filterv_get_xpost(){
	return vpost.x;
}

int16_t filterv_get_ypost(){
	return vpost.y;
}
int16_t filterv_get_zpost(){
	return vpost.z;
}

/*
uint8_t usart_buf[USART_BUF_SIZE];

static void data_to_buf(uint8_t * buf, int16_t data)
{
   buf[0] = data & 0xFF;
   buf[1] = (data >> 8) & 0xFF;
}*/
