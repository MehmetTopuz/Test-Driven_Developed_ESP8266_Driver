/**
 * @file 	esp8266.h
 * @author  Mehmet Topuz
 * @brief   Header file of the AT commands based ESP8266 driver.
 *
 *  Website : <a href="https://mehmettopuz.net/">mehmettopuz.net</a>
 *  Created on: Jan 18, 2022
 *
 */


#ifndef ESP8266_H_
#define ESP8266_H_


/* C++ Detection Block ------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/

#include "ring_buffer.h"

/* AT commands definitions ------------------------------------------------------------------*/

#define AT_CWMODE_STATION			"AT+CWMODE=1\r\n"
#define	AT_CWQAP					"AT+CWQAP\r\n"
#define AT_CWJAP					"AT+CWJAP="

#define AT_CIPCLOSE					"AT+CIPCLOSE\r\n"
#define AT_CIPMUX_SINGLE			"AT+CIPMUX=0\r\n"
#define AT_CIPSTART_TCP				"AT+CIPSTART=\"TCP\","
#define AT_CIPSEND					"AT+CIPSEND="


#define AT_RESPONSE_OK				"OK"
#define AT_RESPONSE_ERROR			"ERROR"
#define AT_RESPONSE_SEND_OK			"SEND OK"
#define AT_RESPONSE_GREATER_THAN	">"

#define TIMEOUT						10000UL	// millisecond

RingBuffer* rx_buffer;			// uart ring buffer structure

/**
 * @brief ESP initialize structure definition
 */
typedef struct
{
    void		(*UART_Transmit)(uint8_t*);			/*!< Driver uses this function pointer to transmit messages over UART.*/
    uint8_t		(*UART_Receive)(void);				/*!< Driver uses this function pointer to receive messages from UART.*/
    uint32_t	(*getTick)(void);					/*!< Driver uses this function pointer to get tick count when calculating timeout.*/

}Esp_Init_Typedef;


typedef enum
{
	FOUND = 0,
	TIMEOUT_ERROR,
	STATUS_OK,
	STATUS_ERROR,
	IDLE,
}Status;

/* Function Prototypes --------------------------------------------------------*/

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
int32_t ESP_Init(void (*UART_Transmit)(uint8_t*),
			 uint8_t (*UART_Receive)(void),
			 uint32_t (*getTick)(void),
			 uint32_t UART_Buffer_Size);
/**
 * @brief 	AT command send function. This function uses UART_Transmit function that is passed
 * 			as a function pointer in the ESP_Init function to send AT commands over UART.
 * @param 	cmd is a string containing the AT command.
 * @retval	None.
 */
void Send_AT_Command(char *cmd);
/**
 * @brief 	This function is used to pass the UART receive data to the ring buffer. User should use
 * 			this function in the  UART ISR.
 * @param 	None.
 * @retval	None.
 */
void ESP_UART_ReceiveHandler(void);

/**
 * @brief 	Read the specified message from the ring buffer.
 * @param 	response is a string is checked if it is in the ring buffer.
 * @retval	1: There is a string passed as a parameter in the ring buffer.
 * @retval	0: There is no string passed as a parameter in the ring buffer.
 */
uint32_t Read_Response(char * response);

/**
 * @brief 	Wait the specified message.
 * @param 	response is a string is checked if it is in the ring buffer.
 * @param 	timeout is an unsigned 32-bit integer that represents timeout in milliseconds.
 * @retval	FOUND			:There is a string passed as a parameter in the ring buffer.
 * @retval	TIMEOUT_ERROR	:It returns when timeout occurs.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Wait_Response(char * response, uint32_t timeout);

/**
 * @brief 	This function handles AT commands to connect the wifi.
 * @param 	ssid is a string that represents the name of your wifi network.
 * @param 	password is a string that represents the password of your wifi network.
 * @retval	STATUS_OK		:Wifi connection is successful.
 * @retval	STATUS_ERROR	:There is an error about connection.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Connect_Wifi(char* ssid, char* password);

/**
 * @brief 	This function handles AT commands to disconnect the wifi.
 * @param 	None.
 * @retval	STATUS_OK		:Disconnect operation is successful.
 * @retval	STATUS_ERROR	:There is an error about disconnect operation.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Disconnect_Wifi(void);

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
Status Command_Process(char **commandArray, char **responseArray, uint8_t numberOfCommands);

/**
 * @brief 	This function handles AT commands to connect TCP server.
 * @param 	ip is a string containing IP address of the server.
 * @param 	port is a string containing port number of the server.
 * @retval	STATUS_OK		:TCP/IP connection is successful.
 * @retval	STATUS_ERROR	:There is an error about connection.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Connect_TCP_Server(char* ip, char* port);

/**
 * @brief 	This function handles AT commands to disconnect TCP server.
 * @param 	None
 * @retval	STATUS_OK		:TCP/IP disconnection is successful.
 * @retval	STATUS_ERROR	:There is an error about disconnection.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Disconnect_TCP_Server(void);

/**
 * @brief 	This function handles AT commands to send messages over TCP/IP.
 * @param 	message is a string wanted to sent over TCP/IP.
 * @retval	STATUS_OK		:The message has been sent successfully.
 * @retval	STATUS_ERROR	:There is an error about sending message.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs. Default timeout is 5000 ms.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE.
 */
Status Send_TCP_Message(char* message);

/**
 * @brief 	This function reads message if there is a message in the buffer received from the TCP/IP server.
 * @param 	receviedMessage : If a message is received, it is assigned to receivedMessage.
 * @retval	STATUS_OK		:The message has been read successfully.
 * @retval	STATUS_ERROR	:There is no TCP message in the buffer.
 */
Status Read_TCP_Message(char* receivedMessage);

/**
 * @brief 	This function waits for a message during the timeout.
 * @param 	receviedMessage :If a message is received, it is assigned to receivedMessage.
 * @param 	timeout is an unsigned 32-bit integer that represents timeout in milliseconds.
 * @retval	STATUS_OK		:The message has been read successfully before timeout does not occur.
 * @retval	STATUS_ERROR	:There is no TCP message in the buffer.
 * @retval	TIMEOUT_ERROR	:It returns TIMEOUT_ERROR when timeout occurs.
 * @retval	IDLE			:If there is not a string in the buffer and timeout does not occur yet, it returns IDLE
 */
Status Wait_TCP_Message(char* receivedMessage, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* ESP8266_H_ */
