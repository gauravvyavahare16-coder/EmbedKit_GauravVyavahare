#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 8
#define SUCCESS 0
#define FAILURE -1

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];

    uint8_t head;
    uint8_t tail;
    uint8_t count;
} RingBuffer;

void ringbuf_init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

uint8_t ringbuf_is_full(RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE);
}

uint8_t ringbuf_is_empty(RingBuffer *rb)
{
    return (rb->count == 0);
}

uint8_t ringbuf_count(RingBuffer *rb)
{
    return rb->count;
}

int ringbuf_write(RingBuffer *rb, uint8_t data)
{
    if (ringbuf_is_full(rb))
    {
        return FAILURE;
    }

    rb->buffer[rb->head] = data;

    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);

    rb->count++;

    return SUCCESS;
}

int ringbuf_read(RingBuffer *rb, uint8_t *data)
{
    if (ringbuf_is_empty(rb))
    {
        return FAILURE;
    }

    *data = rb->buffer[rb->tail];

    rb->tail = (rb->tail + 1) & (BUFFER_SIZE - 1);

    rb->count--;

    return SUCCESS;
}

int main(void)
{
    RingBuffer rb;
    uint8_t data;

    ringbuf_init(&rb);


    uint8_t initial_data[8] =
    {
        0x41, 0x42, 0x43, 0x44,
        0x45, 0x46, 0x47, 0x48
    };

    for (int i = 0; i < 8; i++)
    {
        if (ringbuf_write(&rb, initial_data[i]) == SUCCESS)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)",
                   initial_data[i],
                   ringbuf_count(&rb));

            if (ringbuf_is_full(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }

    if (ringbuf_write(&rb, 0x99) == FAILURE)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

   
    for (int i = 0; i < 3; i++)
    {
        if (ringbuf_read(&rb, &data) == SUCCESS)
        {
            printf("[READ] -> 0x%02X (count=%d)\n",
                   data,
                   ringbuf_count(&rb));
        }
    }


    uint8_t new_data[3] = {0x49, 0x4A, 0x4B};

    for (int i = 0; i < 3; i++)
    {
        if (ringbuf_write(&rb, new_data[i]) == SUCCESS)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)\n",
                   new_data[i],
                   ringbuf_count(&rb));
        }
    }

   
    while (!ringbuf_is_empty(&rb))
    {
        if (ringbuf_read(&rb, &data) == SUCCESS)
        {
            printf("[READ] -> 0x%02X (count=%d)\n",
                   data,
                   ringbuf_count(&rb));
        }
    }

   
    if (ringbuf_read(&rb, &data) == FAILURE)
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}