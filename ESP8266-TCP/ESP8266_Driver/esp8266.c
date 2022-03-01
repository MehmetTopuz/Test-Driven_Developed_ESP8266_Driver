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
#include <string.h>

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
		time = 0;
		return FOUND;
	}
	else if(ESP8266.getTick() - time >= timeout)
	{
		firstCall = 0;
		time = 0;
		return TIMEOUT_ERROR;
	}
	else
	{
		return IDLE;
	}

}

Status Connect_Wifi(char* ssid, char* password)
{
	char *command_buffer[3] =
	{
		AT_CWMODE_STATION,
		AT_CWQAP,
		""
	};
	char *response_buffer[3] =
	{
		AT_RESPONSE_OK,
		AT_RESPONSE_OK,
		AT_RESPONSE_OK
	};
	Status response_state = IDLE;

	char wifi_buffer[50];

	sprintf(wifi_buffer,"%s\"%s\",\"%s\"\r\n",AT_CWJAP,ssid,password); 		// AT+CWJAP="<SSID>","<password>"

	command_buffer[2] = wifi_buffer;

	response_state = Command_Process(command_buffer, response_buffer, 3);

	return response_state;
}

Status Disconnect_Wifi(void)
{
	char *command_buffer[1] = { AT_CWQAP };
	char *response_buffer[1] = { AT_RESPONSE_OK };

	Status response_state = IDLE;

	response_state = Command_Process(command_buffer, response_buffer, 1);

	return response_state;

}

Status Command_Process(char **commandArray, char **responseArray, uint8_t numberOfCommands)
{
	static uint8_t commandFlag  = 1, currentCommand = 0;
	Status response;

	if(!numberOfCommands || currentCommand >= numberOfCommands)
	{
		ringBuffer_flush(rx_buffer);
		return STATUS_ERROR;
	}
	else
	{
		if(commandFlag)
		{
			Send_AT_Command(commandArray[currentCommand]);
			commandFlag = 0;
		}
//		else
//		{
			response = Wait_Response(responseArray[currentCommand], 5000);

			if(Read_Response("ERROR"))
			{
				commandFlag = 1;
				currentCommand = 0;
				ringBuffer_flush(rx_buffer);
				return STATUS_ERROR;
			}
			else if(response == IDLE)
			{
				return response;
			}
			else if(response == FOUND)
			{
				if(currentCommand == (numberOfCommands-1))
				{
					commandFlag = 1;
					currentCommand = 0;
					ringBuffer_flush(rx_buffer);
					return STATUS_OK;
				}
				else
				{
					commandFlag = 1;
					currentCommand += 1;
					ringBuffer_flush(rx_buffer);
					return response;
				}

			}
			else
			{
				commandFlag = 1;
				currentCommand = 0;
				return response;
			}
//		}
	}


}

Status Connect_TCP_Server(char* ip, char* port)
{
	Status response_state = IDLE;

	char *command_buffer[3] =
	{
		AT_CIPCLOSE,
		AT_CIPMUX_SINGLE,
		""
	};

	char *response_buffer[3] =
	{
		AT_RESPONSE_OK,
		AT_RESPONSE_OK,
		AT_RESPONSE_OK
	};

	char wifi_buffer[100];

	sprintf(wifi_buffer,"%s\"%s\",%s\r\n",AT_CIPSTART_TCP,ip,port);

	command_buffer[2] = wifi_buffer;

	response_state = Command_Process(command_buffer, response_buffer, 3);

	return response_state;
}

Status Disconnect_TCP_Server(void)
{
	Status response_state = IDLE;

	char *command_buffer[1] = { AT_CIPCLOSE };
	char *response_buffer[1] = { AT_RESPONSE_OK };

	response_state = Command_Process(command_buffer, response_buffer, 1);

	return response_state;

}

Status Send_TCP_Message(char* message)
{
	Status response_state = STATUS_ERROR;

	uint32_t length=0;

	length = strlen(message);

	char cipSendBuffer[50];

	sprintf(cipSendBuffer,"%s%ld\r\n",AT_CIPSEND,length);

	char *command_buffer[2];

	command_buffer[0] = cipSendBuffer;
	command_buffer[1] = message;

	char *response_buffer[2] =
	{
			AT_RESPONSE_GREATER_THAN,
			AT_RESPONSE_SEND_OK
	};

	response_state = Command_Process(command_buffer, response_buffer, 2);
	return response_state;

}

Status Read_TCP_Message(char* receivedMessage)
{
	Status response_state = IDLE;

	char *substring = strstr((char*)rx_buffer->buffer,"+IPD,");

	char messageLengthString[3];

	uint32_t messageLength=0,index=0;


	if(substring != NULL)
	{
		for(uint8_t i=0;i<3;i++)
		{
			if(*(substring+5+i) == ':')
				break;
			messageLengthString[i] = *(substring+5+i);
			index += 1;

		}

		messageLength = atoi(messageLengthString);

		for(uint32_t i=0;i<=messageLength+1;i++)
		{
			receivedMessage[i] = *(substring+6+index+i);
		}
		ringBuffer_flush(rx_buffer);
		return STATUS_OK;


	}
	else
	{
		ringBuffer_flush(rx_buffer);
		return STATUS_ERROR;
	}


	return response_state;
}

Status Wait_TCP_Message(char* receivedMessage, uint32_t timeout)
{
	static uint8_t firstCall = 1;
	static uint32_t time = 0;

	if(firstCall)
	{
		time = ESP8266.getTick();
		firstCall = 0;
	}

	if(Read_TCP_Message(receivedMessage) == STATUS_OK)
	{
		time = 0;
		firstCall = 1;
		return STATUS_OK;
	}
	else if(ESP8266.getTick()-time >= timeout)
	{
		time = 0;
		firstCall = 1;
		return TIMEOUT_ERROR;
	}
	else
		return IDLE;

}

