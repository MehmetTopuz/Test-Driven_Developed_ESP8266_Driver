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

#define AT_RESPONSE_OK				"OK"
#define AT_RESPONSE_ERROR			"ERROR"



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
	CONNECTION_OK,
	CONNECTION_ERROR,
	IDLE,
}Status;

/* Function Prototypes --------------------------------------------------------*/

int ESP_Init(void (*UART_Transmit)(uint8_t*),
			 uint8_t (*UART_Receive)(void),
			 uint32_t (*getTick)(void),
			 uint32_t UART_Buffer_Size);

void Send_AT_Command(char *cmd);

void ESP_UART_ReceiveHandler(void);

uint32_t Read_Response(char * response);

Status Wait_Response(char * response, uint32_t timeout);

Status Connect_Wifi(char* ssid, char* password);

Status Disconnect_Wifi(void);

Status Command_Process(char** commandArray,uint8_t numberOfCommands);

#ifdef __cplusplus
}
#endif

#endif /* ESP8266_H_ */
