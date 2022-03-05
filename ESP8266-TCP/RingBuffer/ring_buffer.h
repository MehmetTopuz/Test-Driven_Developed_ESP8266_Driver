/**
 * @file 	ring_buffer.h
 * @author  Mehmet Topuz
 * @brief   Header file of the Ring Buffer module.
 *
 *  <a href="https://mehmettopuz.net/">mehmettopuz.net</a>
 *  Created on: Feb 2, 2022
 *
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

/* C++ Detection Block ------------------------------------------------------------------*/
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
/**
 * @brief 	Initialize the ring buffer. Ring buffer is allocated after this function.
 * @param 	size parameter specifies the size of ring buffer.
 * @retval	RingBuffer typedef structure.
 */
RingBuffer* ringBuffer_init(uint32_t size);

/**
 * @brief 	De-initialize the ring buffer.
 * @param 	RingBuffer typedef structure.
 * @retval	None.
 */
void 		ringBuffer_deInit(RingBuffer* buffer);

/**
 * @brief 	Write data into the buffer.
 * @param 	RingBuffer typedef structure.
 * @param 	data is an unsigned 8-bit integer variable to be pushed into the buffer.
 * @retval	None.
 */
void 		ringBuffer_push(RingBuffer* ringBuffer, uint8_t data);

/**
 * @brief 	Push an array into ring buffer.
 * @param 	RingBuffer typedef structure.
 * @param	data is an unsigned 8-bit data array to be written into the buffer.
 * @retval	None.
 */
void 		ringBuffer_pushArray(RingBuffer* ringBuffer, uint8_t* data);

/**
 * @brief 	Read data from the buffer.
 * @param 	RingBuffer typedef structure.
 * @retval	unsigned 8-bit integer data to be read from the buffer.
 */
uint8_t 	ringBuffer_pop(RingBuffer* ringBuffer);

/**
 * @brief 	Clear the ring buffer.
 * @param 	RingBuffer typedef structure.
 * @retval	None.
 */
void 		ringBuffer_flush(RingBuffer* ringBuffer);

/**
 * @brief 	Search a specific data array in the ring buffer .
 * @param 	RingBuffer typedef structure.
 * @param	data is an unsigned 8-bit array to be searched in the buffer.
 * @retval	1: data is available in the buffer.
 * @retval	0: data is not available in the buffer.
 */
uint32_t 	ringBuffer_lookFor(RingBuffer* ringBuffer, uint8_t* data);

/**
 * @brief 	Check the size of the ring buffer.
 * @param 	RingBuffer typedef structure.
 * @retval	Size of ring buffer.
 */
uint32_t 	ringBuffer_capacity(RingBuffer* ringBuffer);

/*End of C++ Detection Block ------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H_ */
/* End of File ------------------------------------------------------------------*/
