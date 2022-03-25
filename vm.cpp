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

        case OP_NIL:
        {
            pushStack(Value(nullptr));
            break;
        }

        case OP_TRUE:
        {
            pushStack(Value(true));
            break;
        }

        case OP_FALSE:
        {
            pushStack(Value(false));
            break;
        }

        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
            binaryOperator(instruction);
            break;

        case OP_NOT:
        {
            pushStack(Value(popStack().isFalsey()));
            break;
        }

        case OP_NEGATE:
        {
            if (!std::holds_alternative<double>(peekStack(0)))
            {
                throw VMRuntimeError("Operand must be a number.");
            }
            const auto topValue = popStack();
            const auto num = std::get<double>(topValue);
            pushStack(Value(-num));
            break;
        }

        case OP_RETURN:
        {
            if (!isStackEmpty())
            {
                std::cout << (std::string)popStack() << std::endl;
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

void VM::binaryOperator(uint8_t op)
{
    if (!peekStack(0).isDouble() || !peekStack(1).isDouble())
    {
        throw VMRuntimeError("Operand must be a number.");
    }
    const auto b = (double)popStack();
    const auto a = (double)popStack();
    switch (op)
    {
    case OP_ADD:
        pushStack(Value(a + b));
        break;
    case OP_SUBTRACT:
        pushStack(Value(a - b));
        break;
    case OP_MULTIPLY:
        pushStack(Value(a * b));
        break;
    case OP_DIVIDE:
        pushStack(Value(a / b));
        break;
    default:
        // TODO: throw runtime exception.
        break;
    };
}

void VM::traceStack()
{
    std::cout << "STCK [";
    for (Value *slot = stack.data(); slot < stackTop; slot++)
    {
        std::cout << (std::string)*slot;
        if (slot + 1 < stackTop)
        {
            std::cout << "|";
        }
    }
    std::cout << "]" << std::endl;
}