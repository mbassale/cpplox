#include "common.h"
#include "chunk.h"

int main(int argc, char *argv[])
{
    Chunk chunk("<code>");
    chunk.write(OP_RETURN, 1);

    auto constant = chunk.writeConstant(1.2);
    chunk.write(OP_CONSTANT, 2);
    chunk.write(constant, 2);

    chunk.write(OP_RETURN, 2);

    chunk.disassemble();
    return EXIT_SUCCESS;
}