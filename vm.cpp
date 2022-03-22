#include "vm.h"

InterpretResult VM::interpret(Chunk &chunk)
{
    if (chunk.size() == 0)
        return InterpretResult::INTERPRET_RUNTIME_ERROR;
    this->chunk = &chunk;
    ip = this->chunk->data();
    return run();
}

InterpretResult VM::run()
{
    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        traceStack();
        chunk->disassembleInstruction((size_t)(ip - chunk->data()));
#endif
        uint8_t instruction;
        switch (instruction = readByte())
        {
        case OP_CONSTANT:
        {
            Value constant = readConstant();
            pushStack(constant);
            break;
        }

        case OP_CONSTANT_LONG:
        {
            Value constant = readConstantLong();
            pushStack(constant);
            break;
        }

        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
            binaryOperator(instruction);
            break;

        case OP_NEGATE:
        {
            pushStack(-popStack());
            break;
        }

        case OP_RETURN:
        {
            if (!isStackEmpty())
            {
                std::cout << popStack() << std::endl;
            }
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

void VM::traceStack()
{
    std::cout << "STCK [";
    for (Value *slot = stack.data(); slot < stackTop; slot++)
    {
        std::cout << *slot;
        if (slot + 1 < stackTop)
        {
            std::cout << "|";
        }
    }
    std::cout << "]" << std::endl;
}