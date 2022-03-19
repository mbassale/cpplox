#include "chunk.h"

Chunk::Chunk() : code()
{
}

void Chunk::write(uint8_t byte)
{
    code.push_back(byte);
}