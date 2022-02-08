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

//char TxString[50];

void UART_Transmit_Fake(uint8_t* data)
{
	mock().actualCall("UART_Transmit_Fake").withParameter("data", data);
	//memcpy(TxString,data,sizeof(data));
}
uint8_t UART_Receive_Fake(void)
{

	return mock().actualCall("UART_Receive_Fake").returnIntValue();
}

uint32_t getTick_Fake(void)
{
	return 1;
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
	mock().expectOneCall("UART_Transmit_Fake").withParameter("data", (uint8_t *)"Test");
	Send_AT_Command((char*)"Test");
	//STRCMP_EQUAL("Test",TxString);
	mock().expectOneCall("UART_Transmit_Fake").withParameter("data", (uint8_t *)"AT\r\n");
	Send_AT_Command((char*)"AT\r\n");
	mock().checkExpectations();

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
	mock().checkExpectations();

}
