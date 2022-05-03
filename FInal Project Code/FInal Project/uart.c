/*
 * uart.c
 *
 * Created: 6/11/2020 9:06:38 PM
 *  Author: Triet Nguyen
 */ 
#include "uart.h"

void UART_Init(uint32_t v_baudRate_u32)
{
	UCSRB= (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);                  // Enable Receiver and Transmitter and Interrupt Receiver
	UCSRC= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);   // Asynchronous mode 8-bit data and 1-stop bit
	UCSRA= 0x00;                                   // Clear the UASRT status register
	UART_SetBaudRate(v_baudRate_u32);
}


void UART_SetBaudRate(uint32_t v_baudRate_u32)
{
	uint16_t RegValue;

	if((v_baudRate_u32 >= C_MinBaudRate_U32) && (v_baudRate_u32<=C_MaxBaudRate_U32))
	{
		/* Check if the requested baudate is within range,
	     If yes then calculate the value to be loaded into baud rate generator. */
		RegValue = M_GetBaudRateGeneratorValue(v_baudRate_u32);
	}
	else
	{
		/*	 Invalid baudrate requested, hence set it to default baudrate of 9600 */
		RegValue = M_GetBaudRateGeneratorValue(9600);
	}

	UBRRL = util_ExtractByte0to8(RegValue);
	UBRRH = util_ExtractByte8to16(RegValue);
}


char UART_RxChar(void)
{
	while(util_IsBitCleared(UCSRA,RXC));  // Wait till the data is received
	return(UDR);                          // return the received char
}

void UART_TxChar(char v_uartData_u8)
{
	while(util_IsBitCleared(UCSRA,UDRE)); // Wait till Transmitter(UDR) register becomes Empty
	UDR =v_uartData_u8;                              // Load the data to be transmitted
}

void UART_TxString(char *ptr_string)
{
	while(*ptr_string)
	UART_TxChar(*ptr_string++);// Loop through the string and transmit char by char
}

uint8_t UART_RxString(char *ptr_string)
{
	char ch;
	uint8_t len = 0;
	while(1)
	{
		ch=UART_RxChar();    //Receive a char
		UART_TxChar(ch);     //Echo back the received char

		if((ch=='\r') || (ch=='\n')) //read till enter key is pressed
		{						     //once enter key is pressed null terminate the string
			ptr_string[len]=0;           //and break the loop
			break;
		}
		else if((ch=='\b') && (len!=0))
		{
			len--;    //If backspace is pressed then decrement the index to remove the old char
		}
		else
		{
			ptr_string[len]=ch; //copy the char into string and increment the index
			len++;
		}
	}
	return len;
}