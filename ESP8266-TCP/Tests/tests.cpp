/**
 * tests.cpp
 *
 *  Created on: Jan 18, 2022
 *      Author: topuz
 */


#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "esp8266.h"
#include "ring_buffer.h"
#include <stdio.h>
#include <string.h>

TEST_GROUP(RingBuffer_Test_Group)
{

	RingBuffer* testBuf;

	void setup()
	{

	   testBuf = ringBuffer_init(50);

	}

	void teardown()
	{

		ringBuffer_deInit(testBuf);
	}
};


TEST(RingBuffer_Test_Group, InitTest)
{

   LONGS_EQUAL(50,testBuf->size);
   CHECK(testBuf->buffer != NULL);
   LONGS_EQUAL(0,testBuf->head);
   LONGS_EQUAL(0,testBuf->tail);

}

TEST(RingBuffer_Test_Group, PushDataTests)
{
	ringBuffer_push(testBuf, 0x45);
	LONGS_EQUAL(0x45,testBuf->buffer[0]);

	ringBuffer_push(testBuf, 0x10);
	LONGS_EQUAL(0x10,testBuf->buffer[1]);

}

TEST(RingBuffer_Test_Group, BufferOverflowTest)
{
	for(int i=testBuf->head;i<(testBuf->head == (testBuf->size - 1));i++)
	{
		ringBuffer_push(testBuf, (uint8_t)i);
	}

	ringBuffer_push(testBuf, 0xFF);			// This data must be the first element of buffer.
	LONGS_EQUAL(0xFF,testBuf->buffer[0]);
}

TEST(RingBuffer_Test_Group, PopDataTests)
{
	ringBuffer_push(testBuf, 0xFF);
	uint8_t data = ringBuffer_pop(testBuf); // 0xFF

	LONGS_EQUAL(0xFF,data);

}

TEST(RingBuffer_Test_Group, PopBufferOverFlowTest)
{
	uint8_t data = 0;

	ringBuffer_push(testBuf, 0xAA);

	for(int i=testBuf->tail;i<(testBuf->tail == (testBuf->size - 1));i++)
	{
		data = ringBuffer_pop(testBuf);
	}

	data = ringBuffer_pop(testBuf);

	LONGS_EQUAL(0xAA,data);
}

TEST(RingBuffer_Test_Group, BufferFlushTest)
{

	for(uint32_t i = 0;i < testBuf->size;i++)  // fill the buffer
	{
		ringBuffer_push(testBuf,(uint8_t)i);
	}
	LONGS_EQUAL(49,testBuf->buffer[testBuf->size - 1]);

	ringBuffer_flush(testBuf);  // clear the buffer

	LONGS_EQUAL(0,testBuf->buffer[testBuf->size - 1]);
	LONGS_EQUAL(0,testBuf->buffer[testBuf->size/2]);    // check random value  (buffer[25]);

//	for(uint32_t i = 0;i < testBuf->size;i++)  // check all elements of the buffer
//	{
//		LONGS_EQUAL(0,testBuf->buffer[i]);
//	}

}

TEST(RingBuffer_Test_Group, PushArrayTest)
{
	ringBuffer_pushArray(testBuf, (uint8_t*)"Test");

	STRCMP_EQUAL("Test",(char*)testBuf->buffer);
}

TEST(RingBuffer_Test_Group, LookForTest)
{
	ringBuffer_pushArray(testBuf, (uint8_t*)"Hello World");

	STRCMP_EQUAL("Hello World",(char*)testBuf->buffer);
	CHECK_EQUAL(1,ringBuffer_lookFor(testBuf, (uint8_t*)"World"));
}

TEST(RingBuffer_Test_Group, CapacityTest)
{

	LONGS_EQUAL(50,ringBuffer_capacity(testBuf));

}


void UART_Transmit_Fake(uint8_t* data)
{
//	mock().actualCall("UART_Transmit_Fake").withParameter("data", data);
	mock().actualCall("UART_Transmit_Fake").withStringParameter("data", (char*)data);
}
uint8_t UART_Receive_Fake(void)
{

	return mock().actualCall("UART_Receive_Fake").returnIntValue();
}

uint32_t time=0;

uint32_t getTick_Fake(void)
{
	if(time == 0xFFFFFFFF)
		time = 0;
	else
		time += 1;
	return time;
}

void (*transmit)(uint8_t*);

TEST_GROUP(EspDriver_Test_Group)
{



	void setup()
	{
		ESP_Init(UART_Transmit_Fake,
				  UART_Receive_Fake,
				  getTick_Fake,
				  100);

	}

	void teardown()
	{

		mock().checkExpectations();
		mock().clear();
	}


};

TEST(EspDriver_Test_Group, Esp_Init_Test)
{

	transmit = UART_Transmit_Fake; // or you can pass UART_Transmit_Fake as a parameter to the function instead of transmit.
	int result = ESP_Init(transmit,
						  UART_Receive_Fake,
						  getTick_Fake,
						  100);

//	int result = ESP_Init(UART_Transmit_Fake,
//						  UART_Receive_Fake,
//						  getTick_Fake,
//	  	  	  	  	  	  100);

	LONGS_EQUAL(1,result);

}

TEST(EspDriver_Test_Group, Send_AT_Command_Test)
{
	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", "Test");
	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", "AT\r\n");

	Send_AT_Command((char*)"Test");
	Send_AT_Command((char*)"AT\r\n");



}

TEST(EspDriver_Test_Group, UART_Receive_Handler_Test)
{

	mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)'O');
	ESP_UART_ReceiveHandler();
	mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)'K');
	ESP_UART_ReceiveHandler();
	mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)'\r');
	ESP_UART_ReceiveHandler();
	mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)'\n');
	ESP_UART_ReceiveHandler();

	STRCMP_EQUAL("OK\r\n",(char*)rx_buffer->buffer);	// check the rx ring buffer.

}

TEST(EspDriver_Test_Group, Read_Response_Test)
{
	char response[10] = "OK\r\n";

	for(int i=0;i<(int)strlen(response);i++)
	{
		mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)response[i]);
		ESP_UART_ReceiveHandler();
	}

	uint32_t result = Read_Response((char*)"OK");

	LONGS_EQUAL(1,result);

}


TEST(EspDriver_Test_Group, Wait_Response_Timeout_Test)
{
	Status response_state = IDLE;

	while(1)
	{
		response_state = Wait_Response((char*)"OK", 1000);
		if(response_state != IDLE)
			break;

	}

	LONGS_EQUAL(TIMEOUT_ERROR,response_state);

}

TEST(EspDriver_Test_Group, Wait_Response_Test)
{

	Status response_state = IDLE;

	char response[10] = "OK\r\n";

	while(1)
	{
		response_state = Wait_Response((char*)"OK",1000);
		if(response_state != IDLE)
			break;
		for(int i=0;i<(int)strlen(response);i++)
		{
			mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)response[i]);
			ESP_UART_ReceiveHandler();
		}
	}

	LONGS_EQUAL(FOUND,response_state);
}

TEST(EspDriver_Test_Group, Connect_Wifi_Timeout_Test)
{
	Status response = TIMEOUT_ERROR;

	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data",AT_CWMODE_STATION);

	while(1)
	{

		response = Connect_Wifi((char*)"SSID",(char*)"1234");

		if(response != IDLE)
		{
			break;
		}

	}


	LONGS_EQUAL(TIMEOUT_ERROR,response);

}

TEST(EspDriver_Test_Group, Connect_Wifi_Error_Test)
{
	char response_arr[3][50] =
	{ "OK\r\n",									// station mode response (AT+CWMODE=1)
	  "OK\r\n",									// check disconnect response (AT+CWQAP)
	  "ERROR\r\n"								// connect wifi command response (AT+CWJAP="SSID","password")
	};

	char fake_tx_buffer[3][50] =
	{
		"AT+CWMODE=1\r\n",
		"AT+CWQAP\r\n",
		"AT+CWJAP=\"SSID\",\"1234\"\r\n"
	};

	for(int i=0;i<3;i++)
	{
		mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", fake_tx_buffer[i]);
	}
	Status response = IDLE;
	int i = 0;
	while(1)
	{
		response = Connect_Wifi((char*)"SSID", (char*)"1234");
		if(response == STATUS_ERROR || response == STATUS_OK || response == TIMEOUT_ERROR)
		{
			break;
		}

		if(i<3)
		{
			for(int j=0;j<(int)strlen(response_arr[i]);j++)
			{
				mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)response_arr[i][j]);
				ESP_UART_ReceiveHandler();
			}
			i++;
		}

	}
	LONGS_EQUAL(STATUS_ERROR,response);

}

TEST(EspDriver_Test_Group, Connect_Wifi_Test)
{
	char response_arr[3][50] =
	{ "OK\r\n",									// station mode response
	  "OK\r\n",								// check wifi connection response
	  "OK\r\n"								// connect wifi command response
	};

	char fake_tx_buffer[3][50] =
	{
		"AT+CWMODE=1\r\n",
		"AT+CWQAP\r\n",
		"AT+CWJAP=\"SSID\",\"1234\"\r\n"
	};

	for(int i=0;i<3;i++)
	{
		mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", fake_tx_buffer[i]);
	}
	Status response;
	int i = 0;
	while(1)
	{
		response = Connect_Wifi((char*)"SSID", (char*)"1234");
		if(response == STATUS_ERROR || response == STATUS_OK || response == TIMEOUT_ERROR)
		{
			break;
		}

		if(i<3)
		{
			for(int j=0;j<(int)strlen(response_arr[i]);j++)
			{
				mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)response_arr[i][j]);
				ESP_UART_ReceiveHandler();
			}
			i++;
		}

	}
	LONGS_EQUAL(STATUS_OK,response);

}

TEST(EspDriver_Test_Group, Disconnect_Wifi_Timeout_Test)
{
	Status response;

	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", AT_CWQAP);

	while(1)
	{
		response = Disconnect_Wifi();

		if(response != IDLE)
			break;

	}

	LONGS_EQUAL(TIMEOUT_ERROR,response);
}

TEST(EspDriver_Test_Group, Disconnect_Wifi_Test)
{
	Status response;

	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", AT_CWQAP);

	while(1)
	{
		response = Disconnect_Wifi();

		if(response != IDLE)
			break;

		for(int j=0;j<(int)strlen(AT_RESPONSE_OK);j++)
		{
			mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)AT_RESPONSE_OK[j]);
			ESP_UART_ReceiveHandler();
		}

	}

	LONGS_EQUAL(STATUS_OK,response);
}

TEST(EspDriver_Test_Group, Command_Process_Test)
{
	char *response_arr[3] =
	{ "OK\r\n",									// station mode response
	  "OK\r\n",								// check wifi connection response
	  "OK\r\n"								// connect wifi command response
	};

	char *fake_command_buffer[3] =
	{
		(char*)"AT+CWMODE=1\r\n",
		(char*)"AT+CWQAP\r\n",
		(char*)"AT+CWJAP=\"SSID\",\"1234\"\r\n"
	};

	for(int i=0;i<3;i++)
	{
		mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", fake_command_buffer[i]);
	}
	Status response;
	int i = 0;
	while(1)
	{
		response = Command_Process(fake_command_buffer, response_arr, 3);
		if(response == STATUS_ERROR || response == STATUS_OK || response == TIMEOUT_ERROR)
		{
			break;
		}

		if(i<3)
		{
			for(int j=0;j<(int)strlen(response_arr[i]);j++)
			{
				mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)response_arr[i][j]);
				ESP_UART_ReceiveHandler();
			}
			i++;
		}

	}
	LONGS_EQUAL(STATUS_OK,response);

}

TEST(EspDriver_Test_Group, Connect_TCP_Test)
{
	char response_arr[3][50] =
	{ "OK\r\n",
	  "OK\r\n",
	  "OK\r\n"
	};

	char *fake_command_buffer[3] =
	{
		(char*)"AT+CIPCLOSE\r\n",
		(char*)"AT+CIPMUX=0\r\n",
		(char*)"AT+CIPSTART=\"TCP\",\"192.168.1.1\",80\r\n"
	};

	for(int i=0;i<3;i++)
	{
		mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", fake_command_buffer[i]);
	}

	Status response;
	int i = 0;
	while(1)
	{
		response = Connect_TCP_Server((char*)"192.168.1.1",(char*)"80");
		if(response == STATUS_ERROR || response == STATUS_OK || response == TIMEOUT_ERROR)
		{
			break;
		}

		if(i<3)
		{
			for(int j=0;j<(int)strlen(response_arr[i]);j++)
			{
				mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)response_arr[i][j]);
				ESP_UART_ReceiveHandler();
			}
			i++;
		}

	}
	LONGS_EQUAL(STATUS_OK,response);

}

TEST(EspDriver_Test_Group, Connect_TCP_Timeout_Test)
{

	char *fake_command_buffer[3] =
	{
		(char*)"AT+CIPCLOSE\r\n",
		(char*)"AT+CIPMUX=0\r\n",
		(char*)"AT+CIPSTART=\"TCP\",\"192.168.1.1\",80\r\n"
	};

	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", fake_command_buffer[0]);


	Status response;
	while(1)
	{
		response = Connect_TCP_Server((char*)"192.168.1.1",(char*)"80");
		if(response == STATUS_ERROR || response == STATUS_OK || response == TIMEOUT_ERROR)
		{
			break;
		}

	}
	LONGS_EQUAL(TIMEOUT_ERROR,response);

}

TEST(EspDriver_Test_Group, Connect_TCP_Error_Test)
{
	char response_arr[3][50] =
	{ "OK\r\n",
	  "OK\r\n",
	  "ERROR\r\n"
	};

	char *fake_command_buffer[3] =
	{
		(char*)"AT+CIPCLOSE\r\n",
		(char*)"AT+CIPMUX=0\r\n",
		(char*)"AT+CIPSTART=\"TCP\",\"192.168.1.1\",80\r\n"
	};

	for(int i=0;i<3;i++)
	{
		mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", fake_command_buffer[i]);
	}

	Status response;
	int i = 0;
	while(1)
	{
		response = Connect_TCP_Server((char*)"192.168.1.1",(char*)"80");
		if(response == STATUS_ERROR || response == STATUS_OK || response == TIMEOUT_ERROR)
		{
			break;
		}

		if(i<3)
		{
			for(int j=0;j<(int)strlen(response_arr[i]);j++)
			{
				mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)response_arr[i][j]);
				ESP_UART_ReceiveHandler();
			}
			i++;
		}

	}
	LONGS_EQUAL(STATUS_ERROR,response);

}

TEST(EspDriver_Test_Group, Disconnect_TCP_Test)
{
	Status response;

	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", AT_CIPCLOSE);

	while(1)
	{
		response = Disconnect_TCP_Server();

		if(response != IDLE)
			break;

		for(int j=0;j<(int)strlen(AT_RESPONSE_OK);j++)
		{
			mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)AT_RESPONSE_OK[j]);
			ESP_UART_ReceiveHandler();
		}

	}

	LONGS_EQUAL(STATUS_OK,response);

}

TEST(EspDriver_Test_Group, Send_TCP_Message_Test)
{
	Status response;
	int i = 0;

	char response_arr[2][50] =
	{ ">\r\n",
	  "SEND OK\r\n",
	};

	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", "AT+CIPSEND=11\r\n"); // AT+CIPSEMD=11
	mock().expectOneCall("UART_Transmit_Fake").withStringParameter("data", "Hello World");
	while(1)
	{
		response = Send_TCP_Message((char*)"Hello World");

		if(response == STATUS_ERROR || response == STATUS_OK || response == TIMEOUT_ERROR)
		{
			break;
		}

		if(i<2)
		{
			for(int j=0;j<(int)strlen(response_arr[i]);j++)
			{
				mock().expectOneCall("UART_Receive_Fake").andReturnValue((int)response_arr[i][j]);
				ESP_UART_ReceiveHandler();
			}
			i++;
		}

	}

	LONGS_EQUAL(STATUS_OK,response);

}

TEST(EspDriver_Test_Group, Read_TCP_Message_Test)
{
	char response[30] = "+IPD,11:Hello World";		// an example data that ESP received from server

	Status response_state = IDLE;
	char received_message[50];

	for(int i=0;i<(int)strlen(response);i++)
	{
		mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)response[i]);
		ESP_UART_ReceiveHandler();
	}

	response_state = Read_TCP_Message(received_message);

	LONGS_EQUAL(STATUS_OK,response_state);
	STRCMP_EQUAL("Hello World",received_message);

}

TEST(EspDriver_Test_Group, Wait_TCP_Message_Timeout_Test)
{

	Status response_state = IDLE;

	char received_message[50];
	while(1)
	{
		response_state = Wait_TCP_Message(received_message, 1000);
		if(response_state != IDLE)
			break;
	}

	LONGS_EQUAL(TIMEOUT_ERROR,response_state);

}

TEST(EspDriver_Test_Group, Wait_TCP_Message_Test)
{
	char response[30] = "+IPD,13:PUMP_MOTOR=ON";		// an example data that ESP received from server

	Status response_state = IDLE;

	char received_message[50];

	while(1)
	{
		response_state = Wait_TCP_Message(received_message, 1000);

		if(response_state != IDLE)
			break;

		for(int i=0;i<(int)strlen(response);i++)
		{
			mock().expectOneCall("UART_Receive_Fake").andReturnValue((uint8_t)response[i]);
			ESP_UART_ReceiveHandler();
		}
	}

	LONGS_EQUAL(STATUS_OK,response_state);
	STRCMP_EQUAL("PUMP_MOTOR=ON",received_message);

}





