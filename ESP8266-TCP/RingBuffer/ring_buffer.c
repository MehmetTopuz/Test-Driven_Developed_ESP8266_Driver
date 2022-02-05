/*
 * ring_buffer.c
 *
 *  Created on: Feb 2, 2022
 *      Author: Topuz
 */


#include "ring_buffer.h"
#include <stdlib.h>
#include <string.h>

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

uint8_t ringBuffer_pop(RingBuffer* ringBuffer)
{
	uint8_t data;

	if(ringBuffer->tail != (ringBuffer->size - 1))
	{
		data = ringBuffer->buffer[ringBuffer->tail];
		ringBuffer->tail += 1;
		return data;
	}
	else
	{

		data = ringBuffer->buffer[ringBuffer->tail];
		ringBuffer->tail = 0;
		return data;
	}

}

void ringBuffer_flush(RingBuffer* ringBuffer)
{
	memset(ringBuffer->buffer,0x00,ringBuffer->size);
}

void ringBuffer_pushArray(RingBuffer* ringBuffer, uint8_t* data)
{
	 for(int i = 0; i<strlen((char*)data);i++)
		 ringBuffer_push(ringBuffer, data[i]);
}

uint32_t ringBuffer_lookFor(RingBuffer* ringBuffer, uint8_t* data)
{
	char* str = strstr((char*)ringBuffer->buffer,(char*)data);

	if(str != NULL)
		return 1;
	else
		return 0;

}

uint32_t ringBuffer_capacity(RingBuffer* ringBuffer)
{
	return ringBuffer->size;
}

