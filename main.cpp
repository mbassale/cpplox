#include "common.h"
#include "chunk.h"

int main(int argc, char *argv[])
{
    Chunk chunk;
    chunk.write(OP_RETURN);
    return EXIT_SUCCESS;
}