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
#include <stdio.h>

static Esp_Init_Typedef ESP8266;



int ESP_Init(void 		(*UART_Transmit)(uint8_t*),
			 uint8_t 	(*UART_Receive)(void),
			 uint32_t 	(*getTick)(void),
			 uint32_t	UART_Buffer_Size)
{
	if(UART_Transmit != NULL		&&
	   UART_Receive	!= NULL			&&
	   getTick != NULL)
	{
		ESP8266.UART_Receive 		= UART_Receive;
		ESP8266.UART_Transmit 		= UART_Transmit;
		ESP8266.getTick 			= getTick;

		rx_buffer = ringBuffer_init(UART_Buffer_Size);

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

void ESP_UART_ReceiveHandler(void)
{
	uint8_t rx_data=0;

	rx_data = ESP8266.UART_Receive();
	ringBuffer_push(rx_buffer, rx_data);
}


uint32_t Read_Response(char * response)
{
	return ringBuffer_lookFor(rx_buffer, (uint8_t*)response);
}


Status Wait_Response(char* response, uint32_t timeout)
{

	static uint32_t time = 0;
	static uint8_t firstCall = 0;

	if(!firstCall)
	{
		time = ESP8266.getTick();
		firstCall = 1;
	}

	if(ringBuffer_lookFor(rx_buffer, (uint8_t*)response))
	{
		firstCall = 0;
		return FOUND;
	}
	else if(ESP8266.getTick() - time >= timeout)
	{
		firstCall = 0;
		return TIMEOUT_ERROR;
	}
	else
	{
		return IDLE;
	}

}

Status Connect_Wifi(char* ssid, char* password)
{
//	static Status response_state = IDLE;
//
//	static uint8_t commandCount = 0,firstCall = 1;


	Send_AT_Command(AT_CWMODE_STATION);
//	switch(commandCount)
//	{
//	case 0:
//		if(firstCall)
//		{
//			Send_AT_Command("AT+CWMODE=1\r\n");  // sending AT+CWMODE=1
//			firstCall = 0;
//		}
//		response_state = Wait_Response(AT_RESPONSE_OK, 1000);
//
//		if(response_state == IDLE)
//		{
//			return IDLE;
//		}
//		else if(response_state == FOUND)
//		{
//			commandCount += 1;
//			ringBuffer_flush(rx_buffer);
//			firstCall = 1;
//		}
//		else if(response_state == TIMEOUT_ERROR)
//		{
//			firstCall = 1;
//			return TIMEOUT_ERROR;
//		}
//		else
//		{
//			firstCall = 1;
//			return CONNECTION_ERROR;
//		}
//
//
//		break;
//	case 1:
//		if(firstCall)
//		{
//			Send_AT_Command(AT_CWQAP);  // sending AT+CWQAP
//			firstCall = 0;
//		}
//		response_state = Wait_Response(AT_RESPONSE_OK, 1000);
//
//		if(response_state == IDLE)
//		{
//			return IDLE;
//		}
//		else if(response_state == FOUND)
//		{
//			commandCount += 1;
//			ringBuffer_flush(rx_buffer);
//			firstCall = 1;
//		}
//		else if(response_state == TIMEOUT_ERROR)
//		{
//			firstCall = 1;
//			return TIMEOUT_ERROR;
//		}
//		else
//		{
//			firstCall = 1;
//			return CONNECTION_ERROR;
//		}
//
//		break;
//
//	case 2:
//	{
//		if(firstCall)
//		{
//			char tx_buffer[100];
//
//			sprintf(tx_buffer,"%s\"%s\",\"%s\"\r\n",AT_CWJAP,ssid,password); 		// AT+CWJAP="<SSID>","<password>"
//
//			Send_AT_Command(tx_buffer);  // sending AT+CWJAP="<SSID>","<password>"
//			firstCall = 0;
//		}
//		response_state = Wait_Response(AT_RESPONSE_OK, 1000);
//
//		if(response_state == IDLE)
//		{
//
//			return IDLE;
//		}
//		else if(response_state == FOUND)
//		{
//			commandCount += 0;
//			ringBuffer_flush(rx_buffer);
//			firstCall = 1;
//			return CONNECTION_OK;
//		}
//		else if(response_state == TIMEOUT_ERROR)
//		{
//			firstCall = 1;
//			return TIMEOUT_ERROR;
//		}
//		else
//		{
//			firstCall = 1;
//			return CONNECTION_ERROR;
//		}
//
//		break;
//	}
//
//	default: return CONNECTION_ERROR;
//	}





	return TIMEOUT_ERROR;
}



