/*-----------------------------Declaration of LIBRARIES--------------------------------*/
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "lcd_lib.h"
#include "uart.h"
#include "stdutils.h"
#include <avr/iom32.h>
////////////////////////////////////////////////////////////////////////////////
/*-----------------------------Declaration of variable------------------------*/
// Global variables declaration
unsigned char PIR_status,temp,PIR_change_flag,PIR_old_status=0;
char uart_variable=0;
unsigned int ADC_result;
char ADC_string   []  ="ADC Value:0000 ";
char lcd_mode=0;
////////////////////////////////////////////////////////////////////////////////
/*-----------------------------Initialize GPIO--------------------------------*/
void GPIO_init()
{
	DDRC=0xF7;
	DDRB=0xF3;
	DDRD=0xFF;
	
}
////////////////////////////////////////////////////////////////////////////////
/*-----------------------------Initialize LCD--------------------------------*/
void init_LCD()
{
	LCD4_init(); _delay_ms(100);
	LCD4_clear(); _delay_ms(100);
}

void display_LCD(unsigned char x, unsigned char y, char *data)
{
	LCD4_gotoxy(x,y);
	LCD4_write_string(data);
}

////////////////////////////////////////////////////////////////////////////////
/*-----------------------------Initialize PIR--------------------------------*/
void PIR_init()
{
	DDRB &=~(1<<3); // MAKE PB3 AS AN INPUT
}

void PIR_status_display()
{
	_delay_ms(1000);
	init_LCD();
	display_LCD(1,1,"PIR STATUS:");
	display_LCD(2,1,"OFF");
	while((PINB&0x08)==0x08)
	{
		init_LCD();
		display_LCD(1,1,"    WARNING");
		display_LCD(2,1,"MOTION DETECTED");
		PORTB|=(1<<PB4);  _delay_ms(300);
		PORTB&=~(1<<PB4);  _delay_ms(300);
	}
}

		



///////////////////////////////////////////////////////////////////////////////
/*------------------------------Initialize Soil Moister (ADC)----------------*/

ISR(ADC_vect)
{
	ADC_result = ADC;
	ADCSRA |= (1<<ADSC) ; // start conversion
}

void Soil_Moister_init()
{
	DDRA &=~ (1<<0); // make PA0 as an input
	ADCSRA= 0x8F;	// 2.56V Vref internal, right justified,
	sei();
	ADMUX = 0xC0;	// select  ADC0 channel 0
	ADCSRA |= (1<<ADSC) ; // start conversion
	
}

void Soil_Moister_data_display()
{
	init_LCD();
	display_LCD(1,1,"Soil Moister: ");
	sprintf(ADC_string,"ADC value:%04d ",ADC_result);
	display_LCD(2,1,ADC_string); _delay_ms(500);
	
}
////////////////////////////////////////////////////////////////////////////////
/*-----------------------------Main Menu -------------------------------------*/
void Green_house_disp()
{
	_delay_ms(1000);
	init_LCD();
	display_LCD(1,1,"Green House ");
	display_LCD(2,1,"Control System ");
}
/*-----------------------------Initialize UART -------------------------------------*/
void UART_display()
{
	UART_TxString("-----------------------------------------------\r");
	UART_TxString("------------------LED CONTROl------------------\r");
	UART_TxString("Enter '1' to toggle LED 1. \r");
	UART_TxString("Enter '2' to toggle LED 2. \r");
	UART_TxString("Enter '3' to toggle LED 3. \r");
	UART_TxString("Enter '4' to toggle LED 4. \r");
	UART_TxString("-----------------------------------------------\r");
}
void control_LED (int led)
{
	PORTD ^= (1<<led); 
	UART_TxString("\r---------------------------------------------\r");
	if (bit_is_set(PIND,4))
		UART_TxString("Led 1: ON \r");
	else UART_TxString("Led 1: OFF \r");
	if (bit_is_set(PIND,5))
	UART_TxString("Led 2: ON \r");
	else UART_TxString("Led 2: OFF \r");
	if (bit_is_set(PIND,6))
	UART_TxString("Led 3: ON \r");
	else UART_TxString("Led 3: OFF \r");
	if (bit_is_set(PIND,7))
	UART_TxString("Led 4: ON \r");
	else UART_TxString("Led 4: OFF \r");
	
}
ISR (USART_RXC_vect)
{
	DDRC |= (1<<0)|(1<<1)|(1<<2); // make PB0,1,2 as an output
	uart_variable = UART_RxChar();
	//UART_TxChar(uart_variable);
	switch(uart_variable)
	{
		case '1':
			control_LED(4);
			break;
		case '2':
			control_LED(5);
			break;
		case '3':
			control_LED(6);
			break;
		case '4':
			control_LED(7);
			break;
	}
}


////////////////////////////////////////////////////////////////////////////////
/*------------------------------Display LCD----------------------------------*/
void data_display (char lcd_mode)
{
	switch(lcd_mode)
	{
		case 0:
			Green_house_disp();
			break;
		case 1:
			PIR_status_display();
			break;
		case 2:
			Soil_Moister_data_display();
			break;
		default:
			Green_house_disp();
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
/*------------------------------Display External interrupt----------------------------------*/
ISR (INT2_vect)
{ 		// ISR for external interrupt 2
	lcd_mode ++;
	if(lcd_mode>=3)
	{
		lcd_mode=0;
		data_display(lcd_mode);
	}
}

void INT2_init()
{
	DDRB &= ~(1<<PB2); // set PB2 as input
	PORTB |= (1<<PB2); // PB2 has Internal Pullup Resiter;
	MCUCSR &=(~(1<<ISC2));  // make INT2 falling edge triggered
	GICR  = (1<<INT2);
	sei ();
}



int main(void)
{
	GPIO_init();
	init_LCD();
	PIR_init();
	Soil_Moister_init();
	INT2_init();
	UART_Init(9600); // Initialize the baud rate
	UART_SetBaudRate(9600); // Set the Baud rate at 9600
	UART_display();
	while (1)
	{
		_delay_ms(500);
		data_display(lcd_mode);
	}
	
}
