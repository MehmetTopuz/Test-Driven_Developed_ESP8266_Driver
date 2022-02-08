/**
 * @file 	esp8266.c
 * @author  Mehmet Topuz
 * @brief   Source file of the AT commands based ESP8266 driver.
 *
 *  Website : <a href="https://mehmettopuz.net/">mehmettopuz.net</a>
 *  Created on: Jan 18, 2022
 *
 */

#include "esp8266.h"
#include <stdlib.h>

static Esp_Init_Typedef ESP8266;

int ESP_Init(void 		(*UART_Transmit)(uint8_t*),
			 uint8_t 	(*UART_Receive)(void),
			 void 		(*UART_ISR)(void),
			 uint32_t 	(*getTick)(void))
{
	if(UART_Transmit != NULL		&&
	   UART_Receive	!= NULL			&&
	   UART_ISR	!= NULL				&&
	   getTick != NULL)
	{
		ESP8266.UART_ISR 			= UART_ISR;
		ESP8266.UART_Receive 		= UART_Receive;
		ESP8266.UART_Transmit 		= UART_Transmit;
		ESP8266.getTick 			= getTick;
		return 1;
	}
	else
	{
		return -1;
	}
}


void Send_AT_Command(char *cmd)
{

	ESP8266.UART_Transmit((uint8_t*)cmd);
}
