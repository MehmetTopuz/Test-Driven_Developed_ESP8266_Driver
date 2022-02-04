/*
 * tests.cpp
 *
 *  Created on: Jan 18, 2022
 *      Author: topuz
 */


#include "CppUTest/TestHarness.h"
#include "esp8266.h"
#include "ring_buffer.h"

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
