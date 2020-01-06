#pragma once

#define QUEUE(TYPE, NAME, SIZE)                                          \
    TYPE NAME[SIZE];                                                     \
    const int NAME##_size = SIZE;                                        \
    static int NAME##_front = 0, NAME##_rear = 0;                        \
    static bool NAME##_full = false;                                     \
    void NAME##_pop()                                                    \
    {                                                                    \
        if (NAME##_full || NAME##_front != NAME##_rear) {                \
            NAME##_rear = ++NAME##_rear < NAME##_size ? NAME##_rear : 0; \
            if (NAME##_full)                                             \
                NAME##_full = false;                                     \
        }                                                                \
    }                                                                    \
    void NAME##_push(TYPE item)                                          \
    {                                                                    \
        NAME[NAME##_front] = item;                                       \
        NAME##_front = (NAME##_front + 1) % NAME##_size;                 \
        if (NAME##_full)                                                 \
            NAME##_rear = NAME##_front;                                  \
        else if (NAME##_rear == NAME##_front)                            \
            NAME##_full = true;                                          \
    }                                                                    \
    bool NAME##_empty()                                                  \
    {                                                                    \
        return (!NAME##_full && NAME##_front == NAME##_rear);            \
    }                                                                    \
    TYPE* NAME##_peek()                                                  \
    {                                                                    \
        if (NAME##_empty())                                              \
            return 0;                                                    \
        return &NAME[NAME##_rear];                                       \
    }                                                                    \
    void NAME##_clear()                                                  \
    {                                                                    \
        NAME##_front = 0;                                                \
        NAME##_rear = 0;                                                 \
        NAME##_full = false;                                             \
    }

/**
 * @brief 可变长度的队列
 * 
 */
#define MIXED_QUEUE(TYPE, NAME, SIZE)                                                                   \
    static uint16_t NAME##_head, NAME##_tail;                                                           \
    static uint8_t NAME##_data[SIZE];                                                                   \
    bool NAME##_empty()                                                                                 \
    {                                                                                                   \
        return NAME##_tail == NAME##_head;                                                              \
    }                                                                                                   \
                                                                                                        \
    void NAME##_dequeue()                                                                               \
    {                                                                                                   \
        if (NAME##_empty())                                                                             \
            return;                                                                                     \
        uint8_t len = NAME##_data[NAME##_head];                                                         \
        NAME##_head = (NAME##_head + len + 1) % SIZE;                                                   \
    }                                                                                                   \
                                                                                                        \
    uint8_t NAME##_peek(uint8_t* pointer)                                                               \
    {                                                                                                   \
        uint8_t len = NAME##_data[NAME##_head];                                                         \
        if (len + NAME##_head >= SIZE) {                                                                \
            uint8_t len1 = SIZE - NAME##_head - 1;                                                      \
            uint8_t len2 = len - len1;                                                                  \
            if (len1 > 0)                                                                               \
                memcpy(pointer, &NAME##_data[NAME##_head + 1], len1);                                   \
            memcpy(&pointer[len1], NAME##_data, len2);                                                  \
        } else {                                                                                        \
            memcpy(pointer, &NAME##_data[NAME##_head + 1], len);                                        \
        }                                                                                               \
        return len;                                                                                     \
    }                                                                                                   \
                                                                                                        \
    void NAME##_enqueue(uint8_t len, uint8_t* data)                                                     \
    {                                                                                                   \
        uint16_t next_tail = (NAME##_tail + len + 1);                                                   \
        while ((NAME##_tail < NAME##_head && next_tail >= NAME##_head)                                  \
            || (NAME##_tail > NAME##_head && next_tail >= SIZE && (next_tail % SIZE) >= NAME##_head)) { \
            NAME##_dequeue();                                                                           \
        }                                                                                               \
        NAME##_data[NAME##_tail] = len;                                                                 \
        if (next_tail > SIZE) {                                                                         \
            uint8_t len1 = SIZE - NAME##_tail - 1;                                                      \
            uint8_t len2 = len - len1;                                                                  \
            if (len1 > 0)                                                                               \
                memcpy(&NAME##_data[NAME##_tail + 1], data, len1);                                      \
            memcpy(NAME##_data, &data[len1], len2);                                                     \
        } else {                                                                                        \
            memcpy(&NAME##_data[NAME##_tail + 1], data, len);                                           \
        }                                                                                               \
        NAME##_tail = next_tail % SIZE;                                                                 \
    }                                                                                                   \
                                                                                                        \
    void NAME##_clear()                                                                                 \
    {                                                                                                   \
        NAME##_tail = 0;                                                                                \
        NAME##_head = 0;                                                                                \
    }
