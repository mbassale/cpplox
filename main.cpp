#include "common.h"
#include "chunk.h"

int main(int argc, char *argv[])
{
    Chunk chunk("<code>");
    chunk.write(OP_RETURN);
    chunk.disassemble();
    return EXIT_SUCCESS;
}