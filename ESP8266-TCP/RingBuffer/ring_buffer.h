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

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */
