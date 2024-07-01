/*
 * i2c.c
 *
 * Created: 10/06/2024 12:17:09
 *  Author: jose
 */ 
#include "i2c.h"
#include <util/twi.h>

void i2c_init() {
	// Set SCL frequency
	TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2;

	// Enable TWI
	TWCR = (1 << TWEN);
}

void i2c_start() {
	// Send start condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	// Wait until transmission completed
	while (!(TWCR & (1 << TWINT)));
}

void i2c_stop() {
	// Send stop condition
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

	// Wait until stop condition is executed and bus released
	while (TWCR & (1 << TWSTO));
}

void i2c_write(uint8_t data) {
	// Load data into data register
	TWDR = data;

	// Start transmission
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait until transmission completed
	while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read_ack() {
	// Start reception, ACK after reception
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	// Wait until data received
	while (!(TWCR & (1 << TWINT)));

	// Return received data
	return TWDR;
}

uint8_t i2c_read_nack() {
	// Start reception, NACK after reception
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait until data received
	while (!(TWCR & (1 << TWINT)));

	// Return received data
	return TWDR;
}