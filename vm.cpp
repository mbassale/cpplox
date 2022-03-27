#include "vm.h"

VM::VM() : chunk{}, ip{}, stack(), objects(), globals()
{
    stackTop = stack.data();
}

VM::~VM()
{
    globals.clear();
    objects.clear();
}

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
        traceGlobals();
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

        case OP_POP:
        {
            popStack();
            break;
        }

        case OP_GET_LOCAL:
        {
            uint8_t slot = readByte();
            pushStack(stack[slot]);
            break;
        }

        case OP_SET_LOCAL:
        {
            uint8_t slot = readByte();
            stack[slot] = peekStack(0);
            break;
        }

        case OP_DEFINE_GLOBAL:
        {
            const auto &symbolName = readConstant();
            Symbol symbol(symbolName);
            globals[symbol] = peekStack(0);
            popStack();
            break;
        }

        case OP_GET_GLOBAL:
        {
            const auto &symbolName = readConstant();
            Symbol symbol(symbolName);
            const auto it = globals.find(symbol);
            if (it == globals.end())
            {
                std::ostringstream ss;
                ss << "Undefined variable: " << symbolName;
                throw VMRuntimeError(ss.str());
            }
            pushStack(it->second);
            break;
        }

        case OP_SET_GLOBAL:
        {
            const auto &symbolName = readConstant();
            Symbol symbol(symbolName);
            const auto it = globals.find(symbol);
            if (it == globals.end())
            {
                std::ostringstream ss;
                ss << "Undefined variable: " << symbolName;
                throw VMRuntimeError(ss.str());
            }
            it->second = peekStack(0);
            break;
        }

        case OP_EQUAL:
        {
            const auto a = popStack();
            const auto b = popStack();
            pushStack(Value(a == b));
            break;
        }

        case OP_GREATER:
        case OP_LESS:
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

        case OP_PRINT:
        {
            std::cout << popStack() << std::endl;
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

void VM::binaryOperator(uint8_t op)
{
    // String concatenation
    if (peekStack(0).isString() || peekStack(1).isString())
    {
        const auto b = (std::string)popStack();
        const auto a = (std::string)popStack();
        if (op == OP_ADD)
        {
            pushStack(Value(a + b));
            return;
        }
    }

    // Number operation
    if (!peekStack(0).isDouble() || !peekStack(1).isDouble())
    {
        throw VMRuntimeError("Operands must be numbers or strings.");
    }
    const auto b = (double)popStack();
    const auto a = (double)popStack();
    switch (op)
    {
    case OP_GREATER:
        pushStack(Value(a > b));
        break;
    case OP_LESS:
        pushStack(Value(a < b));
        break;
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

void VM::traceGlobals()
{
    std::cout << "GLOBALS {\n";
    auto it = globals.begin();
    while (it != globals.end())
    {
        const auto &[symbol, value] = *it++;
        std::cout << symbol << ": " << (std::string)value << "\n";
    }
    std::cout << "}" << std::endl;
}