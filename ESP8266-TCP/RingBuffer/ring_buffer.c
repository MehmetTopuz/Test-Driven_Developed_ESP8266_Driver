/*
 * ring_buffer.c
 *
 *  Created on: Feb 2, 2022
 *      Author: Topuz
 */


#include "ring_buffer.h"
#include <stdlib.h>


RingBuffer* ringBuffer_init(uint32_t size)
{
	RingBuffer* rBuf;

	rBuf = (RingBuffer*)malloc(1*sizeof(RingBuffer));
	if(rBuf == NULL)
	{
		// Error Checks
		return 0;
	}

	rBuf->size = size;
	rBuf->head = 0;
	rBuf->tail = 0;

	rBuf->buffer = (uint8_t*)calloc((size_t)size,sizeof(uint8_t));
	if(rBuf->buffer == NULL)
		{
			// Error Checks
			return 0;
		}

	return rBuf;
}

void ringBuffer_deInit(RingBuffer* buffer)
{
	if(buffer != NULL)
	{
		if(buffer->buffer != NULL)
		{
			free(buffer->buffer);
		}
		free(buffer);
	}
}

void ringBuffer_push(RingBuffer* ringBuffer, uint8_t data)
{
	if(ringBuffer->head != (ringBuffer->size - 1))
	{
		ringBuffer->buffer[ringBuffer->head] = data;
		ringBuffer->head += 1;
	}
	else
	{
		ringBuffer->buffer[ringBuffer->head] = data;
		ringBuffer->head = 0;
	}
}



