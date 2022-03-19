#ifndef __cpplox_chunk_h
#define __cpplox_chunk_h

#include "common.h"

typedef enum
{
    OP_RETURN,
} OpCode;

class Chunk
{
private:
    std::vector<uint8_t> code;

public:
    Chunk();
    void write(uint8_t byte);
};

#endif // __cpplox_chunk_h