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


RingBuffer* rx_buffer;

typedef struct
{
    void		(*UART_Transmit)(uint8_t*);
    uint8_t		(*UART_Receive)(void);
    uint32_t	(*getTick)(void);

}Esp_Init_Typedef;

typedef enum
{
	FOUND = 0,
	NOT_FOUND,
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
 * 			-1: There is an error caused by function pointers or memory allocation.
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
 * 			0: There is no string passed as a parameter in the ring buffer.
 */
uint32_t Read_Response(char * response);

Status Wait_Response(char * response, uint32_t timeout);

Status Connect_Wifi(char* ssid, char* password);

Status Disconnect_Wifi(void);

Status Command_Process(char **commandArray, char **responseArray, uint8_t numberOfCommands);

Status Connect_TCP_Server(char* ip, char* port);

Status Disconnect_TCP_Server(void);

Status Send_TCP_Message(char* message);

Status Read_TCP_Message(char* receivedMessage);

Status Wait_TCP_Message(char* receivedMessage, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* ESP8266_H_ */
