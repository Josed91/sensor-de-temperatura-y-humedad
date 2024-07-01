/*
 * rtc.h
 *
 * Created: 10/06/2024 12:16:53
 *  Author: jose
 */ 


#ifndef RTC_H_
#define RTC_H_

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define DS3232_ADDR_WRITE 0xD0
#define DS3232_ADDR_READ  0xD1

void ds3232_init();
void ds3232_set_fecha(uint8_t dia, uint8_t mes, uint16_t anio);
void ds3232_set_time(uint8_t hour, uint8_t minute, uint8_t second);
void ds3232_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second);
void DS3232_GetDate(uint8_t *dia, uint8_t *mes, uint16_t *anio);


#endif /* RTC_H_ */