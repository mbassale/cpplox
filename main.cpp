#include "common.h"
#include "chunk.h"
#include "vm.h"

int main(int argc, char *argv[])
{
    VM vm;
    Chunk chunk("<main>");

    auto constant = chunk.writeConstant(1.2);
    chunk.write(OP_CONSTANT);
    chunk.write(constant);

    auto constant2 = chunk.writeConstant(1227);
    chunk.write(OP_CONSTANT_LONG, 2);

    uint32bytes tmp = {0};
    tmp.u32 = constant2;
    chunk.write(tmp.bytes.b0);
    chunk.write(tmp.bytes.b1);
    chunk.write(tmp.bytes.b2);

    chunk.write(OP_NEGATE, 3);

    chunk.write(OP_ADD, 3);

    auto constant3 = chunk.writeConstant(2);
    chunk.write(OP_CONSTANT);
    chunk.write(constant3);

    chunk.write(OP_DIVIDE);

    chunk.write(OP_RETURN, 3);

    chunk.disassemble();

    std::cout << "== execution ==" << std::endl;
    vm.interpret(chunk);
    return EXIT_SUCCESS;
}