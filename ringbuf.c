#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 8

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} RingBuffer;

// Initialize buffer
void ringbuf_init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

// Check empty
uint8_t ringbuf_is_empty(const RingBuffer *rb)
{
    return (rb->count == 0);
}

// Check full 
uint8_t ringbuf_is_full(const RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE);
}

// Get count
uint8_t ringbuf_count(const RingBuffer *rb)
{
    return rb->count;
}

// Write one byte 
int ringbuf_write(RingBuffer *rb, uint8_t data)
{
    if (ringbuf_is_full(rb))
    {
        return -1;
    }

    rb->buffer[rb->head] = data;

    /*
     * Faster than % BUFFER_SIZE.
     * Works only because BUFFER_SIZE = 8
     * a power of two.
     */
    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);

    rb->count++;

    return 0;
}

// Read one byte 

int ringbuf_read(RingBuffer *rb, uint8_t *data)
{
    if (ringbuf_is_empty(rb))
    {
        return -1;
    }

    *data = rb->buffer[rb->tail];

    rb->tail = (rb->tail + 1) & (BUFFER_SIZE - 1);

    rb->count--;

    return 0;
}

int main(void)
{
    RingBuffer rb;
    uint8_t data;

    ringbuf_init(&rb);

    printf("-------- STEP 1 --------\n");

    uint8_t first_data[] =
    {
        0x41, 0x42, 0x43, 0x44,
        0x45, 0x46, 0x47, 0x48
    };

    for (int i = 0; i < 8; i++)
    {
        if (ringbuf_write(&rb, first_data[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)%s\n",
                   first_data[i],
                   ringbuf_count(&rb),
                   ringbuf_is_full(&rb) ? " FULL" : "");
        }
    }

    printf("\n-------- STEP 2 --------\n");

    if (ringbuf_write(&rb, 0x99) != 0)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    printf("\n-------- STEP 3 --------\n");

    for (int i = 0; i < 3; i++)
    {
        if (ringbuf_read(&rb, &data) == 0)
        {
            printf("[READ] -> 0x%02X (count=%u)\n",
                   data,
                   ringbuf_count(&rb));
        }
    }

    printf("\n-------- STEP 4 --------\n");

    uint8_t second_data[] = {0x49, 0x4A, 0x4B};

    for (int i = 0; i < 3; i++)
    {
        if (ringbuf_write(&rb, second_data[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)%s\n",
                   second_data[i],
                   ringbuf_count(&rb),
                   ringbuf_is_full(&rb) ? " FULL" : "");
        }
    }

    printf("\n-------- STEP 5 --------\n");

    while (!ringbuf_is_empty(&rb))
    {
        ringbuf_read(&rb, &data);

        printf("[READ] -> 0x%02X (count=%u)\n",
               data,
               ringbuf_count(&rb));
    }

    printf("\n-------- STEP 6 --------\n");

    if (ringbuf_read(&rb, &data) != 0)
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0; 
}