/*
 * dht11.h
 *
 * Created: 3/6/2024 15:35:59
 *  Author: Barcala
 */ 


#ifndef DHT11_H_
#define DHT11_H_

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
// Definir el pin al que está conectado el DHT11
#define DHT11_PIN PC0
#define DHT11_DDR DDB0

// Buffer para almacenar los datos del DHT11
uint8_t data[5];

// Prototipo
uint8_t dht11_read(uint8_t *temperature, uint8_t *humidity);
#endif /* DHT11_H_ */
