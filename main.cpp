#include "common.h"
#include "chunk.h"

int main(int argc, char *argv[])
{
    Chunk chunk("<code>");
    chunk.write(OP_RETURN);

    auto constant = chunk.writeConstant(1.2);
    chunk.write(OP_CONSTANT);
    chunk.write(constant);

    chunk.disassemble();
    return EXIT_SUCCESS;
}