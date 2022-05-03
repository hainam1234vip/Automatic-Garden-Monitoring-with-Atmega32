/*
 * uart.h
 *
 * Created: 6/11/2020 9:06:20 PM
 *  Author: Triet Nguyen
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr\io.h>
#include "stdutils.h"

/***************************************************************************************************
                             Baudrate configurations
***************************************************************************************************/
#define C_MinBaudRate_U32 2400
#define C_MaxBaudRate_U32 115200UL


#define M_GetBaudRateGeneratorValue(baudrate)  (((F_CPU -((baudrate) * 8L)) / ((baudrate) * 16UL)))
/**************************************************************************************************/

void UART_Init(uint32_t v_baudRate_u32);
void UART_SetBaudRate(uint32_t v_baudRate_u32);
void UART_TxChar(char v_uartData_u8);
char UART_RxChar(void);
void UART_TxString(char *ptr_string);
uint8_t UART_RxString(char *ptr_string);



#endif /* UART_H_ */