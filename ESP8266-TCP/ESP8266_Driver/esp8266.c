/**
 * @file 	esp8266.c
 * @author  Mehmet Topuz
 * @brief   Source file of the basic ESP8266 driver. This driver uses AT commands to communicate with the ESP.
 * 			This driver does not support all features of ESP8266 for now. You can use this driver
 * 			to connect to TCP server and read or send messages. Also this driver uses ring buffer to handle
 * 			UART receive operations.
 *
 *  Website : <a href="https://mehmettopuz.net/">mehmettopuz.net</a>
 *  Created on: Jan 18, 2022
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "esp8266.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* Private Variables ------------------------------------------------------------------*/
static Esp_Init_Typedef ESP8266;

/* Functions ------------------------------------------------------------------*/

/**
 * @brief 	Initializing the ESP.
 * @param 	UART_Transmit parameter is a function pointer that is used to transmit
 * 			unsigned 8-bit UART data array.
 * @param 	UART_Receive parameter is a function pointer that is used to receive
 * 			unsigned 8-bit UART data.
 * @param 	getTick parameter is a function pointer that is used to calculate timeout.
 * @param 	UART_Buffer_Size parameter is used by ring buffer to allocate buffer.
 * @retval	1 : There is no error. Initializing is successful.
 * @retval -1 : There is an error caused by function pointers or memory allocation.
 */
int32_t ESP_Init(void 		(*UART_Transmit)(uint8_t*),
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

		if(rx_buffer != NULL)
			return 1;
		else
			return -1;
	}
	else
	{
		return -1;
	}
}

/**
 * @brief 	AT command send function. This function uses UART_Transmit function that is passed
 * 			as a function pointer in the ESP_Init function to send AT commands over UART.
 * @param 	cmd is a string containing the AT command.
 * @retval	None.
 */
void Send_AT_Command(char *cmd)
{

	ESP8266.UART_Transmit((uint8_t*)cmd);
}
/**
 * @brief 	This function is used to pass the UART receive data to the ring buffer. User should use
 * 			this function in the UART ISR.
 * @param 	None.
 * @retval	None.
 */
void ESP_UART_ReceiveHandler(void)
{
	ringBuffer_push(rx_buffer, ESP8266.UART_Receive());
}

/**
 * @brief 	Read the specified message from the ring buffer.
 * @param 	response is a string is checked if it is in the ring buffer.
 * @retval	1: There is a string passed as a parameter in the ring buffer.
 * @retval	0: There is no string passed as a parameter in the ring buffer.
 */
uint32_t Read_Response(char * response)
{
	return ringBuffer_lookFor(rx_buffer, (uint8_t*)response);
}

/**
 * @brief 	Wait the specified message.
 * @param 	response is a string is checked if it is in the ring buffer.
 * @param 	timeout is an unsigned 32-bit integer that represents timeout in milliseconds.
 * @retval	FOUND			:There is a string passed as a parameter in the ring buffer.
 * @retval	TIMEOUT_ERROR	:It returns when timeout occurs.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
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

/**
 * @brief 	This function handles AT commands to connect the wifi.
 * @param 	ssid is a string that represents the name of your wifi network.
 * @param 	password is a string that represents the password of your wifi network.
 * @retval	STATUS_OK		:Wifi connection is successful.
 * @retval	STATUS_ERROR	:There is an error about connection.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
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

/**
 * @brief 	This function handles AT commands to disconnect the wifi.
 * @param 	None.
 * @retval	STATUS_OK		:Disconnect operation is successful.
 * @retval	STATUS_ERROR	:There is an error about disconnect operation.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Disconnect_Wifi(void)
{
	char *command_buffer[1] = { AT_CWQAP };
	char *response_buffer[1] = { AT_RESPONSE_OK };

	Status response_state = IDLE;

	response_state = Command_Process(command_buffer, response_buffer, 1);

	return response_state;

}

/**
 * @brief 	This function handles AT commands and waits the messages expected from the ESP8266.
 * @param 	commandArray is a two dimensional string array containing AT commands.
 * @param 	responseArray is a two dimensional string array containing response messages expected from the ESP8266.
 * @param 	numberOfCommands parameter represents the number of commands passed as a parameter in the commandArray.
 * @retval	STATUS_OK		:It returns when ESP send the OK message.
 * @retval	STATUS_ERROR	:It returns when ESP send the ERROR message or number of commands is equal zero.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
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
			response = Wait_Response(responseArray[currentCommand], TIMEOUT);

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
					return IDLE;
				}

			}
			else
			{
				commandFlag = 1;
				currentCommand = 0;
				ringBuffer_flush(rx_buffer);
				return response;
			}
	}


}

/**
 * @brief 	This function handles AT commands to connect TCP server.
 * @param 	ip is a string containing IP address of the server.
 * @param 	port is a string containing port number of the server.
 * @retval	STATUS_OK		:TCP/IP connection is successful.
 * @retval	STATUS_ERROR	:There is an error about connection.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Connect_TCP_Server(char* ip, char* port)
{
	Status response_state = IDLE;

	char *command_buffer[2] =
	{
		AT_CIPMUX_SINGLE,
		""
	};

	char *response_buffer[2] =
	{
		AT_RESPONSE_OK,
		AT_RESPONSE_OK
	};

	char wifi_buffer[100];

	sprintf(wifi_buffer,"%s\"%s\",%s\r\n",AT_CIPSTART_TCP,ip,port);

	command_buffer[1] = wifi_buffer;

	response_state = Command_Process(command_buffer, response_buffer, 2);

	return response_state;
}

/**
 * @brief 	This function handles AT commands to disconnect TCP server.
 * @param 	None
 * @retval	STATUS_OK		:TCP/IP disconnection is successful.
 * @retval	STATUS_ERROR	:There is an error about disconnection.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Disconnect_TCP_Server(void)
{
	Status response_state = IDLE;

	char *command_buffer[1] = { AT_CIPCLOSE };
	char *response_buffer[1] = { AT_RESPONSE_OK };

	response_state = Command_Process(command_buffer, response_buffer, 1);

	return response_state;

}

/**
 * @brief 	This function handles AT commands to send messages over TCP/IP.
 * @param 	message is a string wanted to sent over TCP/IP.
 * @retval	STATUS_OK		:The message has been sent successfully.
 * @retval	STATUS_ERROR	:There is an error about sending message.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
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

/**
 * @brief 	This function reads message if there is a message in the buffer received from the TCP/IP server.
 * @param 	receviedMessage : If a message is received, it is assigned to receivedMessage.
 * @retval	STATUS_OK		:The message has been read successfully.
 * @retval	STATUS_ERROR	:There is no TCP message in the buffer.
 */
Status Read_TCP_Message(char* receivedMessage)
{

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

		for(uint32_t i=0;i<messageLength;i++)
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



}

/**
 * @brief 	This function waits for a message during the timeout.
 * @param 	receviedMessage :If a message is received, it is assigned to receivedMessage.
 * @param 	timeout is an unsigned 32-bit integer that represents timeout in milliseconds.
 * @retval	STATUS_OK		:The message has been read successfully before timeout does not occur.
 * @retval	STATUS_ERROR	:There is no TCP message in the buffer.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE
 */
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

