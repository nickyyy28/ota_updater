#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include <stdint.h>
#include "uni_shell.h"

#define RING_BUFFER_MAX_SIZE 	1024

typedef struct {
	uint8_t buffer[RING_BUFFER_MAX_SIZE];
	uint32_t capacity;
	uint32_t size;
	uint32_t head;
	uint32_t tail;
}RingBuffer;

void ring_buffer_init(RingBuffer *buffer);

/**
 * @brief push data to ring buffer tail, move tail index
 * 
 * @param buffer 
 * @param data 
 * @param len 
 * @return uint8_t 
 */
uint8_t ring_buffer_write(RingBuffer *buffer, const uint8_t* data, uint32_t len);

/**
 * @brief read data from ring buffer head, move head index
 * 
 * @param buffer 
 * @param dst 
 * @param len 
 * @return uint8_t 
 */
uint8_t ring_buffer_read(RingBuffer *buffer, uint8_t *dst, uint32_t len);

/**
 * @brief get ring buffer data size
 * 
 * @param buffer 
 * @return uint32_t 
 */
inline uint32_t ring_buffer_size(RingBuffer *buffer);

/**
 * @brief read data from ring buffer head, but dont move head index
 * 
 * @param buffer 
 * @param dst 
 * @param len 
 * @return uint8_t 
 */
uint8_t ring_buffer_view_data(RingBuffer *buffer, uint8_t *dst, uint32_t len);

#endif
