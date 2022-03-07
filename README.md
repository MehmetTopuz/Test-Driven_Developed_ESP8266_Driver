# Test-Driven Developed ESP8266 Driver

In this project, I implemented a demo driver to communicate between ESP-01 and STM32G474(NUCLEO) following the principles of Test-Driven Development.
I have used CppUTest that is an unit testing framework to test the codes.

## How to Use

First of all, you have to implement your own UART receive, UART transmit and getTick functions because this driver needs these functions to communicate with ESP-01.

```c
void UART_SendMessage(uint8_t* messageArray)
{

	while(*messageArray)
	{
		USART1->TDR = *messageArray++;
		while(!(USART1->ISR & (1<<6)));		// wait for transmit register(TC) to set.
	}
/* with HAL drivers-----------------------*/
//	HAL_UART_Transmit(&huart1, messageArray, strlen((char*)messageArray), HAL_MAX_DELAY);
}
uint8_t UART_ReceiveByte(void)
{

	return USART1->RDR;
/* with HAL drivers-----------------------*/
//	uint8_t buffer[10];
//
//	HAL_UART_Receive(&huart1, &buffer, 1, HAL_MAX_DELAY);
//
//	return buffer[0];
}

```
Pass the functions to the ESP_Init() function as a function pointer. I have used HAL_GetTick() instead of implementing my own getTick function.
```c
  ESP_Init(UART_SendMessage,	// UART transmit function
		  UART_ReceiveByte,		// UART receive function
		  HAL_GetTick,			// get tick function
		  255					// UART ring buffer size
		  );
```

Add ESP_UART_ReceiveHandler() function to ISR.

```c
void USART1_IRQHandler(void)
{
	  if(!(USART2->ISR & (1<<5)))			// rx interrupt
	  {
		 ESP_UART_ReceiveHandler(); 		// ESP receive handler function.
	  }
}
```

It is ready to use. You can use this driver to connect TCP/IP server and send/read messages for now. I will continue to improve the driver as soon as possible.
Here is an example how to connect TCP/IP server.
```c
  while(!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin));		// Start button

  USART1->CR1 |= (1<<5); // rx interrupt enable

  ESP_Init(UART_SendMessage,	// UART transmit function
		  UART_ReceiveByte,		// UART receive function
		  HAL_GetTick,			// get tick function
		  255					// UART ring buffer size
		  );

  while(1)
  {
	  Status connectionStatus = Connect_Wifi("SSID", "Password");

	  if(connectionStatus == STATUS_OK)
		  break;
	  else if(connectionStatus == STATUS_ERROR || connectionStatus == TIMEOUT_ERROR)
	  {
		  Error_Handler();
	  }
  }

  while(Connect_TCP_Server("192.168.88.183", "255") == IDLE);

  while(Send_TCP_Message("Test message") == IDLE);

  char receivedCommand[50];
  while (1)
  {
	  if(Read_TCP_Message(receivedCommand) == STATUS_OK)
	  {

		  if(strcmp(receivedCommand,"MOTOR_ON") == 0)
		  {
			  while(Send_TCP_Message("MOTOR_ON command has been detected.\n") == IDLE);

			  	  	  	  	  	  // do something else
		  }
		  else if(strcmp(receivedCommand,"MOTOR_OFF") == 0)
		  {
			  while(Send_TCP_Message("MOTOR_OFF command has been detected.\n") == IDLE);
			  	  	  	  	  	  	  	  // do something else
		  }
		  else if(strcmp(receivedCommand,"LED_ON") == 0)
		  {
			  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		  }
		  else if(strcmp(receivedCommand,"LED_OFF") == 0)
		  {
			  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
		  }
		  else
		  {
			  while(Send_TCP_Message("Invalid command!\n") == IDLE);
		  }
		  memset(receivedCommand,0,50);		// clear the received command buffer
	  }

	  HAL_Delay(100);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```

