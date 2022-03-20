#include "vm.h"

InterpretResult VM::interpret(Chunk &chunk)
{
    this->chunk = &chunk;
    ip = this->chunk->data();
    return run();
}

InterpretResult VM::run()
{
    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        chunk->disassembleInstruction((size_t)(ip - chunk->data()));
#endif
        uint8_t instruction;
        switch (instruction = readByte())
        {
        case OP_CONSTANT:
        {
            Value constant = readConstant();
            std::cout << constant << std::endl;
            break;
        }

        case OP_CONSTANT_LONG:
        {
            Value constant = readConstantLong();
            std::cout << constant << std::endl;
            break;
        }

        case OP_RETURN:
        {
            return InterpretResult::INTERPRET_OK;
            break;
        }

        default:
        {
            break;
        }
        }
    }

    return InterpretResult::INTERPRET_RUNTIME_ERROR;
}