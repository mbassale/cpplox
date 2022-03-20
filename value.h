#ifndef __cpplox_value_h
#define __cpplox_value_h

#include "common.h"

union uint32bytes
{
    uint32_t u32;
    struct _bytes
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } bytes;
};

typedef double Value;

#endif // __cpplox_value_h