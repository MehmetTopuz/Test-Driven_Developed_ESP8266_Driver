/**
 * @file 	ring_buffer.h
 * @author  Mehmet Topuz
 * @brief   Header file of Ring Buffer module.
 *
 *  <a href="https://mehmettopuz.net/">mehmettopuz.net</a>
 *  Created on: Feb 2, 2022
 *
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/**
 * @brief Ring Buffer structure definition
 */
typedef struct
{
	uint32_t head;				/*!< Specifies the head position of ring buffer. */
	uint32_t tail;				/*!< Specifies the tail position of ring buffer. */
	uint32_t size;				/*!< Specifies the size of ring buffer. */
	uint8_t* buffer;			/*!< Specifies the ring buffer to be allocated. */
}RingBuffer;

/* Function Prototypes -------------------------------------------------------*/
RingBuffer* ringBuffer_init(uint32_t size);
void 		ringBuffer_deInit(RingBuffer* buffer);
void 		ringBuffer_push(RingBuffer* ringBuffer, uint8_t data);
void 		ringBuffer_pushArray(RingBuffer* ringBuffer, uint8_t* data);
uint8_t 	ringBuffer_pop(RingBuffer* ringBuffer);
void 		ringBuffer_flush(RingBuffer* ringBuffer);
uint32_t 	ringBuffer_lookFor(RingBuffer* ringBuffer, uint8_t* data);
uint32_t 	ringBuffer_capacity(RingBuffer* ringBuffer);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */
