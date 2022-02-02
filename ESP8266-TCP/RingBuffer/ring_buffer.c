/*
 * ring_buffer.c
 *
 *  Created on: Feb 2, 2022
 *      Author: Topuz
 */


#include "ring_buffer.h"



RingBuffer* ringBuffer_init(uint32_t size)
{
	RingBuffer* rBuf;

	rBuf->size = size;

	return rBuf;
}
