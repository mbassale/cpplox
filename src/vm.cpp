#include "vm.h"

VM::VM() : stack(), frames(), frameCount(0), objects(), globals() {
  stackTop = stack.data();
}

VM::~VM() {
  globals.clear();
  objects.clear();
}

InterpretResult VM::interpret(FunctionPtr function) {
  if (function->getChunk().size() == 0)
    return InterpretResult::INTERPRET_RUNTIME_ERROR;
  frameCount = 0;
  resetStack();
  pushStack(function);
  ClosurePtr closure = std::make_shared<Closure>(function);
  popStack();
  pushStack(closure);
  call(closure, 0);
  const auto result = run();
  resetStack();
  return result;
}

InterpretResult VM::run() {
  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    traceInstruction();
#endif
    uint8_t instruction;
    switch (instruction = readByte()) {
      case OP_CONSTANT: {
        Value constant = readConstant();
        pushStack(constant);
        break;
      }

      case OP_NIL: {
        pushStack(Value(nullptr));
        break;
      }

      case OP_TRUE: {
        pushStack(Value(true));
        break;
      }

      case OP_FALSE: {
        pushStack(Value(false));
        break;
      }

      case OP_POP: {
        popStack();
        break;
      }

      case OP_GET_LOCAL: {
        uint8_t slot = readByte();
        pushStack(getFrame().fp[slot]);
        break;
      }

      case OP_SET_LOCAL: {
        uint8_t slot = readByte();
        getFrame().fp[slot] = peekStack(0);
        break;
      }

      case OP_DEFINE_GLOBAL: {
        const auto &symbolName = readConstant();
        Symbol symbol(symbolName);
        globals[symbol] = peekStack(0);
        popStack();
        break;
      }

      case OP_GET_GLOBAL: {
        const auto &symbolName = readConstant();
        Symbol symbol(symbolName);
        const auto it = globals.find(symbol);
        if (it == globals.end()) {
          std::ostringstream ss;
          ss << "Undefined variable: " << symbolName;
          runtimeError(ss.str());
        }
        pushStack(it->second);
        break;
      }

      case OP_SET_GLOBAL: {
        const auto &symbolName = readConstant();
        Symbol symbol(symbolName);
        const auto it = globals.find(symbol);
        if (it == globals.end()) {
          std::ostringstream ss;
          ss << "Undefined variable: " << symbolName;
          runtimeError(ss.str());
        }
        it->second = peekStack(0);
        break;
      }

      case OP_GET_UPVALUE: {
        uint8_t slot = readByte();
        auto &upvalues = getFrame().closure->getUpvalues();
        auto *value = upvalues[slot]->getLocation();
        pushStack(*value);
        break;
      }

      case OP_SET_UPVALUE: {
        uint8_t slot = readByte();
        auto &upvalues = getFrame().closure->getUpvalues();
        auto *value = upvalues[slot]->getLocation();
        *value = peekStack(0);
        break;
      }

      case OP_EQUAL: {
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

      case OP_NOT: {
        pushStack(Value(popStack().isFalsey()));
        break;
      }

      case OP_NEGATE: {
        if (!std::holds_alternative<double>(peekStack(0))) {
          runtimeError("Operand must be a number.");
        }
        const auto topValue = popStack();
        const auto num = std::get<double>(topValue);
        pushStack(Value(-num));
        break;
      }

      case OP_PRINT: {
        std::cout << popStack() << std::endl;
        break;
      }

      case OP_JUMP: {
        uint16_t offset = readShort();
        getFrame().ip += offset;
        break;
      }

      case OP_JUMP_IF_FALSE: {
        uint16_t offset = readShort();
        if (peekStack(0).isFalsey()) getFrame().ip += offset;
        break;
      }

      case OP_LOOP: {
        uint16_t offset = readShort();
        getFrame().ip -= offset;
        break;
      }

      case OP_CALL: {
        const auto argCount = readByte();
        if (!callValue(peekStack(argCount), argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }

      case OP_CLOSURE: {
        const auto functionValue = readConstant();
        if (!functionValue.isObject()) {
          runtimeError("Operand must be an object.");
        }
        const auto functionObjPtr = (ObjectPtr)functionValue;
        if (functionObjPtr->getType() != ObjectType::OBJ_FUNCTION) {
          runtimeError("Operand must be a function.");
        }
        const auto functionPtr =
            std::dynamic_pointer_cast<Function>(functionObjPtr);
        ClosurePtr closure = std::make_shared<Closure>(functionPtr);
        pushStack(Value(closure));

        auto &upvalues = closure->getUpvalues();
        for (int i = 0; i < upvalues.size(); i++) {
          uint8_t isLocal = readByte();
          uint8_t index = readByte();
          if (isLocal) {
            upvalues[i] = captureUpvalue(getFrame().fp + index);
          } else {
            upvalues[i] = getFrame().closure->getUpvalues()[index];
          }
        }
        break;
      }

      case OP_RETURN: {
        auto result = popStack();
        popFrame();
        if (frameCount == 0) {
          popStack();
          return InterpretResult::INTERPRET_OK;
        }
        pushStack(result);
        break;
      }

      default: {
        break;
      }
    }
  }

  return InterpretResult::INTERPRET_RUNTIME_ERROR;
}

bool VM::callValue(Value callee, int argCount) {
  if (callee.isObject()) {
    const auto object = (ObjectPtr)callee;
    switch (object->getType()) {
      case ObjectType::OBJ_CLOSURE: {
        const auto closure = std::dynamic_pointer_cast<Closure>(object);
        // we have a closure?
        if (closure) {
          return call(closure, argCount);
        }
        break;
      }
      case ObjectType::OBJ_NATIVE: {
        const auto nativeFunction =
            std::dynamic_pointer_cast<NativeFunction>(object);
        // we have a native function?
        if (nativeFunction) {
          const auto functionPtr = nativeFunction->getFunctionPtr();
          const auto result = functionPtr(argCount, stackTop - argCount);
          pushStack(result);
          return true;
        }
        break;
      }
      default:
        break;
    }
  }
  runtimeError("Can only call function and classes.");
  return false;
}

bool VM::call(ClosurePtr closure, int argCount) {
  if (argCount != closure->getArity()) {
    std::ostringstream ss;
    ss << "Expected " << closure->getArity() << " arguments but got "
       << argCount;
    runtimeError(ss.str());
  }
  if (frameCount == FRAMES_MAX) {
    runtimeError("Stack overflow.");
  }
  pushFrame(closure, argCount);
  return true;
}

UpvaluePtr VM::captureUpvalue(Value *local) {
  auto upvalue = std::make_shared<Upvalue>(local);
  return upvalue;
}

void VM::binaryOperator(uint8_t op) {
  // String concatenation
  if (peekStack(0).isString() || peekStack(1).isString()) {
    const auto b = (std::string)popStack();
    const auto a = (std::string)popStack();
    if (op == OP_ADD) {
      pushStack(Value(a + b));
      return;
    }
  }

  // Number operation
  if (!peekStack(0).isDouble() || !peekStack(1).isDouble()) {
    runtimeError("Operands must be numbers or strings.");
  }
  const auto b = (double)popStack();
  const auto a = (double)popStack();
  switch (op) {
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

void VM::defineNative(const std::string &name, NativeFnPtr function) {
  Symbol nativeFunctionName(name);
  const auto nativeFunction = std::make_shared<NativeFunction>(name, function);
  globals[nativeFunctionName] = Value(nativeFunction);
}

void VM::traceStack() {
  std::cout << "STCK [";
  for (Value *slot = stack.data(); slot < stackTop; slot++) {
    std::cout << (std::string)*slot;
    if (slot + 1 < stackTop) {
      std::cout << "|";
    }
  }
  std::cout << "]" << std::endl;
}

void VM::traceGlobals() {
  std::cout << "GLOBALS {\n";
  auto it = globals.begin();
  while (it != globals.end()) {
    const auto &[symbol, value] = *it++;
    std::cout << symbol << ": " << (std::string)value << "\n";
  }
  std::cout << "}" << std::endl;
}

void VM::runtimeError(const std::string &message) {
  std::ostringstream ss;
  ss << message << std::endl;
  for (int i = frameCount - 1; i >= 0; i--) {
    const auto &frame = frames[i];
    FunctionPtr function = frame.closure->getFunction();
    auto &function_chunk = function->getChunk();
    size_t instruction = frame.ip - function_chunk.data() - 1;
    ss << "[line " << function_chunk.getLines().get(instruction) << "] in "
       << function->toString() << std::endl;
  }
  throw VMRuntimeError(ss.str());
}

void VM::traceInstruction() {
  traceStack();
  traceGlobals();
  Disassembler disassembler(getFrame().getChunk());
  auto insOffset = (size_t)(getFrame().ip - getFrame().getChunk().data());
  const auto ins = disassembler.disassembleInstruction(insOffset);
  OpCodePrinter opCodePrinter(getFrame().getChunk());
  opCodePrinter.printInstruction(ins);
}