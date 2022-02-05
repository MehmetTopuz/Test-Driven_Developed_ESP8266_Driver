/*
 * ring_buffer.h
 *
 *  Created on: Feb 2, 2022
 *      Author: Topuz
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef struct
{
	uint32_t head;
	uint32_t tail;
	uint32_t size;
	uint8_t* buffer;
}RingBuffer;


RingBuffer* ringBuffer_init(uint32_t size);
void ringBuffer_deInit(RingBuffer* buffer);
void ringBuffer_push(RingBuffer* ringBuffer, uint8_t data);
void ringBuffer_pushArray(RingBuffer* ringBuffer, uint8_t* data);
uint8_t ringBuffer_pop(RingBuffer* ringBuffer);
void ringBuffer_flush(RingBuffer* ringBuffer);
uint32_t ringBuffer_lookFor(RingBuffer* ringBuffer, uint8_t* data);
uint32_t ringBuffer_capacity(RingBuffer* ringBuffer);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */
