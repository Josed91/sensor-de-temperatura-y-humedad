/*
 * i2c.h
 *
 * Created: 10/06/2024 12:16:36
 *  Author: jose
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

#define F_CPU 16000000UL
#define SCL_CLOCK 100000L

void i2c_init();
void i2c_start();
void i2c_stop();
void i2c_write(uint8_t data);
uint8_t i2c_read_ack();
uint8_t i2c_read_nack();



#endif /* I2C_H_ */