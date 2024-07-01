/*
 * rtc.c
 *
 * Created: 10/06/2024 12:17:21
 *  Author: jose
 */ 
#include "rtc.h"
#include "i2c.h"

void ds3232_init() {
	// Initialize I2C communication
	i2c_init();
}
void ds3232_set_fecha(uint8_t dia, uint8_t mes, uint16_t anio) {
	// Configurar registros de fecha
	uint8_t fecha_data[3] ;//= {0x04, dia, mes, (uint8_t)(anio & 0xFF)}; // LSB del año
	fecha_data[0]=((dia/10)<<4) |(dia%10);
	fecha_data[1]=((mes/10)<<4) |(mes%10);
	fecha_data[2]=(uint8_t)(((anio/10)<<4 )| (anio%10));
	i2c_start();
	i2c_write(DS3232_ADDR_WRITE);
	i2c_write(0x04); // Comenzar en la dirección 0x04 para los registros de fecha
	for (uint8_t i = 0; i < 3; i++) {
		i2c_write(fecha_data[i]);
	}
	i2c_stop();
}

void ds3232_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
	// Set time registers
	uint8_t time_data[3];
		//uint8_t time_data[4] = {0x00, minute, hour, second};
	time_data[0]=((second/10)<<4) |(second%10);
	time_data[1]=((minute/10)<<4) |(minute%10);
	time_data[2]=((hour/10)<<4) |(hour%10);
	i2c_start();
	i2c_write(DS3232_ADDR_WRITE);
	i2c_write(0x00); // Start at address 0x00
	for (uint8_t i = 0; i < 3; i++) {
		i2c_write(time_data[i]);
	}
	i2c_stop();
}

void ds3232_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second) {
	// Read time registers
	i2c_start();
	i2c_write(DS3232_ADDR_WRITE);
	i2c_write(0x00); // Start at address 0x00
	i2c_stop();
	i2c_start();
	i2c_write(DS3232_ADDR_READ);
	*second = i2c_read_ack();
	*minute = i2c_read_ack();
	*hour = i2c_read_nack();
	i2c_stop();
}

void DS3232_GetDate(uint8_t *dia, uint8_t *mes, uint16_t *anio) {
	// Leer registros de fecha
	i2c_start();
	i2c_write(DS3232_ADDR_WRITE);
	i2c_write(0x04); // Comenzar en la dirección 0x04 para los registros de fecha
	i2c_stop();
	
	i2c_start();
	i2c_write(DS3232_ADDR_READ);
	*dia = i2c_read_ack();
	*mes = i2c_read_ack();
	//*anio = i2c_read_ack(); // Leer el byte MSB del año
	*anio = i2c_read_nack(); // Leer el byte LSB del año y combinar con MSB
	
	i2c_stop();
}