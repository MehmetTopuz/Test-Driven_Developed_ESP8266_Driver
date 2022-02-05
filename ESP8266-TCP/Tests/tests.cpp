/*
 * tests.cpp
 *
 *  Created on: Jan 18, 2022
 *      Author: topuz
 */


#include "CppUTest/TestHarness.h"
#include "esp8266.h"
#include "ring_buffer.h"
#include <stdio.h>

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

	CHECK_EQUAL(1,ringBuffer_lookFor(testBuf, (uint8_t*)"World"));
}


