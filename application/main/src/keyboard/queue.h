#pragma once

#define QUEUE(TYPE, NAME, SIZE) \
    TYPE NAME[SIZE];\
    const int NAME##_size = SIZE;\
    static int NAME##_front = 0, NAME##_rear = 0;\
    static bool NAME##_full = false;\
    static void NAME##_pop()\
    { \
        if (NAME##_full || NAME##_front != NAME##_rear) {\
            NAME##_rear = ++NAME##_rear < NAME##_size ? NAME##_rear : 0;\
            if (NAME##_full) NAME##_full = false;\
        }\
    }\
    static void NAME##_push(TYPE item) \
    {\
        NAME[NAME##_front] = item;\
        NAME##_front = (NAME##_front + 1) % NAME##_size;\
        if (NAME##_full) NAME##_rear = NAME##_front;\
        else if (NAME##_rear == NAME##_front) NAME##_full = true;\
    }\
    static bool NAME##_empty() \
    {\
        return (!NAME##_full && NAME##_front == NAME##_rear);\
    }\
    static TYPE* NAME##_peek() \
    {\
        if (NAME##_empty()) return 0;\
        return &NAME[NAME##_rear];\
    }\
    static void NAME##_clear()\
    {\
        NAME##_front = 0; NAME##_rear = 0; NAME##_full = false;\
    }
