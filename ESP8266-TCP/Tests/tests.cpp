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

};


TEST(RingBuffer_Test_Group, InitTest)
{
   RingBuffer* testBuf;

   testBuf = ringBuffer_init(50);

   LONGS_EQUAL(50,testBuf->size);
   CHECK(testBuf->buffer != NULL);
   LONGS_EQUAL(0,testBuf->head);
   LONGS_EQUAL(0,testBuf->tail);

}
