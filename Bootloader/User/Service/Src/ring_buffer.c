#include "ring_buffer.h"

#include <string.h>

void ring_buffer_init(RingBuffer *buffer)
{
    memset(buffer->buffer, 0, RING_BUFFER_MAX_SIZE);
    buffer->capacity = RING_BUFFER_MAX_SIZE;
    buffer->head = buffer->tail = 0;
    buffer->size = 0;
}

uint8_t ring_buffer_write(RingBuffer *buffer, const uint8_t* data, uint32_t len)
{
    if (buffer->capacity - buffer->size < len) {
        return 1;
    }

    if (buffer->capacity - buffer->tail >= len) {
        memcpy(&buffer->buffer[buffer->tail], data, len);
    } else {
        uint32_t temp = buffer->capacity - buffer->tail;
        memcpy(&buffer->buffer[buffer->tail], data, temp);
        memcpy(&buffer->buffer[0], data + temp, len - temp);
    }
    buffer->tail += len;
    buffer->tail %= buffer->capacity;
    buffer->size += len;
    return 0;
}

uint8_t ring_buffer_read(RingBuffer *buffer, uint8_t *dst, uint32_t len)
{
    if (buffer->size < len) {
        return 1;
    }

    if (buffer->capacity - buffer->head >= len) {
        memcpy(dst, &buffer->buffer[buffer->head], len);
    } else {
        uint32_t temp = buffer->capacity - buffer->head;
        memcpy(dst, &buffer->buffer[buffer->head], temp);
        memchr(dst + temp, &buffer->buffer[0], len - temp);
    }
    buffer->head += len;
    buffer->head %= buffer->capacity;
    buffer->size -= len;
    return 0;
}

inline uint32_t ring_buffer_size(RingBuffer *buffer)
{
    return buffer->size;
}

uint8_t ring_buffer_view_data(RingBuffer *buffer, uint8_t *dst, uint32_t len)
{
    if (buffer->size < len) {
        return 1;
    }

    if (buffer->capacity - buffer->head >= len) {
        memcpy(dst, &buffer->buffer[buffer->head], len);
    } else {
        uint32_t temp = buffer->capacity - buffer->head;
        memcpy(dst, &buffer->buffer[buffer->head], temp);
        memcpy(dst + temp, &buffer->buffer[0], len - temp);
    }
    return 0;
}

