/**
 * @file 	ring_buffer.c
 * @author  Mehmet Topuz
 * @brief   Source file of Ring Buffer module.
 *
 *  <a href="https://mehmettopuz.net/">mehmettopuz.net</a>
 *  Created on: Feb 2, 2022
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "ring_buffer.h"
#include <stdlib.h>
#include <string.h>

/* Functions ------------------------------------------------------------------*/

/**
 * @brief 	Initialize the ring buffer. Ring buffer is allocated after this function.
 * @param 	size parameter specifies the size of ring buffer.
 * @retval	Pointer of the ring buffer structure.
 */
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

/**
 * @brief 	Deinitialize the ring buffer.
 * @param 	Ring buffer typedef structure.
 * @retval	None.
 */
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

/**
 * @brief 	Write data into the buffer.
 * @param 	Ring buffer typedef structure.
 * @param 	unsigned 8-bit integer data to be pushed into the buffer.
 * @retval	None.
 */
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

/**
 * @brief 	Read data from the buffer.
 * @param 	Ring buffer typedef structure.
 * @retval	unsigned 8-bit integer data to be read from the buffer.
 */
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

/**
 * @brief 	Clear the ring buffer.
 * @param 	Ring buffer typedef structure.
 * @retval	None.
 */
void ringBuffer_flush(RingBuffer* ringBuffer)
{
	memset(ringBuffer->buffer,0x00,ringBuffer->size);
}

/**
 * @brief 	Add an array into ring buffer.
 * @param 	Ring buffer typedef structure.
 * @param	unsigned 8-bit data array to be written into the buffer.
 * @retval	None.
 */
void ringBuffer_pushArray(RingBuffer* ringBuffer, uint8_t* data)
{
	 for(int i = 0; i<strlen((char*)data);i++)
		 ringBuffer_push(ringBuffer, data[i]);
}

/**
 * @brief 	Search a specific data array in the ring buffer .
 * @param 	Ring buffer typedef structure.
 * @param	Unsigned 8-bit data array to be searched in the buffer.
 * @retval	1: data array is available in the buffer.
 * 			0: data array is not available in the buffer.
 */
uint32_t ringBuffer_lookFor(RingBuffer* ringBuffer, uint8_t* data)
{
	char* str = strstr((char*)ringBuffer->buffer,(char*)data);

	if(str != NULL)
		return 1;
	else
		return 0;

}

/**
 * @brief 	Check the size of the ring buffer.
 * @param 	Ring buffer typedef structure.
 * @retval	Ring buffer size.
 */
uint32_t ringBuffer_capacity(RingBuffer* ringBuffer)
{
	return ringBuffer->size;
}

