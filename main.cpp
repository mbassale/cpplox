#include "common.h"
#include "chunk.h"
#include "vm.h"

int main(int argc, char *argv[])
{
    VM vm;
    Chunk chunk("<main>");

    auto constant = chunk.writeConstant(1.2);
    chunk.write(OP_CONSTANT, 2);
    chunk.write(constant, 2);

    auto constant2 = chunk.writeConstant(1227);
    chunk.write(OP_CONSTANT_LONG, 3);

    uint32bytes tmp = {0};
    tmp.u32 = constant2;
    chunk.write(tmp.bytes.b0, 3);
    chunk.write(tmp.bytes.b1, 3);
    chunk.write(tmp.bytes.b2, 3);

    chunk.write(OP_RETURN, 2);

    chunk.disassemble();

    vm.interpret(chunk);
    return EXIT_SUCCESS;
}