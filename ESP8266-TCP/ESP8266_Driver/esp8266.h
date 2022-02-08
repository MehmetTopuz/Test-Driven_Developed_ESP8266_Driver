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


typedef struct
{
    void		(*UART_Transmit)(uint8_t*);
    uint8_t		(*UART_Receive)(void);
    void		(*UART_ISR)(void);
    uint32_t	(*getTick)(void);

}Esp_Init_Typedef;

/* Function Prototypes --------------------------------------------------------*/

int ESP_Init(void (*UART_Transmit)(uint8_t*),
			 uint8_t (*UART_Receive)(void),
			 void (*UART_ISR)(void),
			 uint32_t (*getTick)(void));

void Send_AT_Command(char *cmd);



#ifdef __cplusplus
}
#endif

#endif /* ESP8266_H_ */
