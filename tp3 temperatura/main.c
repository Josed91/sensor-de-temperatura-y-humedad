/*
 * tp3 temperatura.c
 *
 * Created: 29/5/2024 13:29:11
 * Author : Barcala
 */ 
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "serialPort.h"
#include "rtc.h"
#define F_CPU 16000000UL

#define BR9600 (0x67)	// 0x67=103 configura BAUDRATE=9600@16MHz

//mensaje
char msg_bienvenida[]= "Bienvenido al sensor de temperatura ambiente\n\r";
char msg[] = "se pauso o reanudo el programa!\n\r";

//comunicación con la ISR UART
volatile char RX_Buffer=0;
volatile uint8_t flag_mostrar_info=1;
volatile uint8_t mostrar_pausa = 0;

//Control de Temporizacion
volatile uint8_t flag_tiempo=0;
volatile uint8_t cont_tick_tiempo=0;


// Función de inicialización del temporizador
void Temporizador_timer0_init() {
	// Configurar el modo CTC
	TCCR0A |= (1 << WGM01);

	// Configurar el preescalador a F_CPU/1024
	TCCR0B |= (1 << CS02) | (1 << CS00);

	// Calcular el valor de comparación para generar una interrupción cada 10 ms
	// F_CPU = 16,000,000 Hz (frecuencia de reloj del microcontrolador)
	// Prescaler = 1024
	// Tiempo deseado = 10 ms
	// Valor de comparación = (F_CPU * Tiempo deseado) / (Prescaler * 1000) - 1
	OCR0A = 155; //(F_CPU * 10/ (1024 * 1000)) - 1;

	// Habilitar la interrupción de comparación de salida del Timer0
	TIMSK0 |= (1 << OCIE0A);
}


int main(void)
{
	ds3232_init();
	unsigned char temperatura, humedad;
	uint8_t day=1, month=7, year=24, hour=13, minute=59, second=50;
	char temp_str[5], hum_str[5], date_str[11], time_str[9];
	
	Temporizador_timer0_init();
	SerialPort_Init(BR9600); 		// Inicializo formato 8N1 y BAUDRATE = 9600bps
	SerialPort_TX_Enable();			// Activo el Transmisor del Puerto Serie
	SerialPort_RX_Enable();			// Activo el Receptor del Puerto Serie
	SerialPort_RX_Interrupt_Enable();	// Activo Interrupción de recepcion.
	sei();								// Activo la mascara global de interrupciones (Bit I del SREG en 1)
	uint8_t result;
	ds3232_set_fecha(day,month,year);
	ds3232_set_time(hour,minute,second);
	
	SerialPort_Send_String(msg_bienvenida);
	
	while(1)
	{
		if (flag_tiempo){
			flag_tiempo=0;
			
			DS3232_GetDate(&day, &month, &year);
			ds3232_get_time(&hour, &minute, &second);
			result=dht11_read(&temperatura, &humedad);
			
			
			// Separar la parte alta (decenas) de la hora y la parte baja (unidades)
			uint8_t hourh = hour >> 4; // Desplazar 4 bits a la derecha para obtener la parte alta (decenas)
			uint8_t hourb = hour & 0x0F; // Aplicar una máscara de bits para obtener la parte baja (unidades)

			// Convertir la hora de BCD (Binary-Coded Decimal) a decimal
			hour = hourb + (10 * hourh); // Combinar la parte alta y baja para obtener el valor decimal de la hora

			// Convertir los minutos de BCD a decimal
			minute = ((minute >> 4) * 10) + (minute & 0x0F); // Separar decenas y unidades, luego combinarlas

			// Convertir los segundos de BCD a decimal
			second = ((second >> 4) * 10) + (second & 0x0F); // Separar decenas y unidades, luego combinarlas

			// Convertir el día de BCD a decimal
			day = ((day >> 4) * 10) + (day & 0x0F); // Separar decenas y unidades, luego combinarlas

			// Convertir el mes de BCD a decimal
			month = ((month >> 4) * 10) + (month & 0x0F); // Separar decenas y unidades, luego combinarlas

			// Convertir el año de BCD a decimal
			year = ((year >> 4) * 10) + (year & 0x0F); // Separar decenas y unidades, luego combinarlas

			//Fin de correcciones de formato de fecha y hora
			
			if(flag_mostrar_info) {
				if (result == 0) {
					// Convierte temperatura and humidity to strings
					sprintf(temp_str, "%d", temperatura);
					sprintf(hum_str, "%d", humedad);
					sprintf(date_str, "%02d/%02d/%02d", day, month, year);
					sprintf(time_str, "%02d:%02d:%02d", hour, minute, second);

					// Transmitir datos por UART
					SerialPort_Send_String("Fecha: ");
					SerialPort_Send_String(date_str);
					SerialPort_Send_String(" Hora: ");
					SerialPort_Send_String(time_str);
					SerialPort_Send_String(" - Temperatura: ");
					SerialPort_Send_String(temp_str);
					SerialPort_Send_String("°C Humedad: ");
					SerialPort_Send_String(hum_str);
					SerialPort_Send_String("%\n\r");
				}
			}
			else{	// Eco:
				SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisión este libre (bloqueante)
				SerialPort_Send_Data(RX_Buffer);			  // Envío el dato recibido
				
				RX_Buffer=0;
				
				if (mostrar_pausa){
					SerialPort_Send_String("\n\r");
					SerialPort_Send_String(msg);
					mostrar_pausa=0;
				}
				
			}
			
			
		}
						
					
	}
	
	
	return 0;
}

// Rutina de interrupción del temporizador
ISR(TIMER0_COMPA_vect) {
	if (++cont_tick_tiempo == 198){
		flag_tiempo=1;
		cont_tick_tiempo=0;
	}
}

// Rutina de Servicio de Interrupción de Byte Recibido
ISR(USART_RX_vect){
	RX_Buffer = UDR0; //la lectura del UDR borra flag RXC
	
	//Verifico si se presiono S o s
	if((RX_Buffer == 's') || (RX_Buffer == 'S')){
		if(flag_mostrar_info)
			flag_mostrar_info=0;
		else
			flag_mostrar_info=1;
		mostrar_pausa=1;
	}
}

