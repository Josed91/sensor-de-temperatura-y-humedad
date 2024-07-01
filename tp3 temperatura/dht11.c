/*
 * dht.c
 *
 * Created: 3/6/2024 15:32:35
 *  Author: Barcala
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include "dht11.h"
#define DHT11_TIMEOUT       100

uint8_t dht11_read(uint8_t *temperature, uint8_t *humidity) {
	uint8_t bits[5];
	uint8_t i, j = 0;

	// Initialize data buffers
	*temperature = *humidity = 0;

	// Request sample
	DDRC |= (1 << DHT11_PIN);
	PORTC &= ~(1 << DHT11_PIN);
	_delay_ms(18);
	PORTC |= (1 << DHT11_PIN);
	_delay_us(1);
	DDRC &= ~(1 << DHT11_PIN);
	_delay_us(39);

	// Check start condition 1
	if ((PINC & (1 << DHT11_PIN))) {
		return 1; // Error
	}
	_delay_us(80);
	// Check start condition 2
	if (!(PINC & (1 << DHT11_PIN))) {
		return 2; // Error
	}
	_delay_us(80);

	// Read data
	for (j = 0; j < 5; j++) {
		uint8_t result = 0;
		for (i = 0; i < 8; i++) {
			while (!(PINC & (1 << DHT11_PIN))); // Wait for the pin to go high
			_delay_us(30); // Delay for 30 us
			if (PINC & (1 << DHT11_PIN)) // If the pin is high after delay
			result |= (1 << (7 - i)); // Write 1 to result
			while (PINC & (1 << DHT11_PIN)); // Wait for the pin to go low
		}
		bits[j] = result;
	}

	// Check checksum
	if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) != bits[4]) {
		return 3; // Error
	}

	// Convert data
	*humidity = bits[0];
	*temperature = bits[2];

	return 0; // OK
}