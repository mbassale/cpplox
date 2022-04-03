#include "compiler.h"

Compiler::Compiler()
{
    scopeDepth = 0;
    hadError = false;
    panicMode = false;
    locals.reserve(UINT8_MAX + 1);
}

Compiler::Compiler(const Compiler &compiler) : scanner(compiler.scanner), current(compiler.current), previous(compiler.previous), locals(compiler.locals)
{
    scopeDepth = 0;
    hadError = false;
    panicMode = false;
}

FunctionPtr Compiler::compile(const std::string &name, const std::string &source)
{
    function = std::make_shared<Function>(FunctionType::TYPE_SCRIPT, name);
    scanner = std::make_shared<Scanner>(source);
    advance();

    ParseFnArgs args;
    args.canAssign = true;
    while (!match(TOKEN_EOF))
    {
        declaration(args);
    }

    emitEpilogue();
    return hadError ? nullptr : std::move(function);
}

FunctionPtr Compiler::compileFunction(Compiler &compiler, FunctionType &functionType, const std::string &name)
{
    function = std::make_shared<Function>(functionType, name);
    scanner = compiler.scanner;
    beginScope();
    consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
    if (!check(TOKEN_RIGHT_PAREN))
    {
        do
        {
            auto arity = function->incrArity();
            if (arity > 255)
            {
                errorAtCurrent("Can't have more than 255 parameters.");
            }
            uint8_t param = parseVariable("Expect parameter name.");
            defineVariable(param);
        } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
    block();
    emitReturn();
    return function;
}

void Compiler::advance()
{
    previous = current;
    for (;;)
    {
        current = scanner->next();
        std::cout << current.str() << std::endl;
        if (current.type != TOKEN_ERROR)
            break;
        errorAtCurrent(current.lexeme());
    }
}

bool Compiler::match(TokenType type)
{
    if (!check(type))
        return false;
    advance();
    return true;
}
bool Compiler::check(TokenType type)
{
    return current.type == type;
}

void Compiler::declaration(const ParseFnArgs &args)
{
    if (match(TOKEN_FUN))
    {
        funDeclaration(args);
    }
    else if (match(TOKEN_VAR))
    {
        varDeclarationStatement(args);
    }
    else
    {
        statement(args);
    }

    if (panicMode)
        synchronize();
}

void Compiler::funDeclaration(const ParseFnArgs &args)
{
    uint8_t global = parseVariable("Expect function name.");
    markInitialized();
    const auto functionName = (std::string)currentChunk().readConstant(global);
    compileDefinition(TYPE_FUNCTION, functionName);
    defineVariable(global);
}

void Compiler::varDeclarationStatement(const ParseFnArgs &args)
{
    uint8_t global = parseVariable("Expect variable name.");

    if (match(TOKEN_EQUAL))
    {
        expression(args);
    }
    else
    {
        emitByte(OP_NIL);
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

    defineVariable(global);
}

void Compiler::statement(const ParseFnArgs &args)
{
    if (match(TOKEN_PRINT))
    {
        printStatement(args);
    }
    else if (match(TOKEN_FOR))
    {
        forStatement(args);
    }
    else if (match(TOKEN_IF))
    {
        ifStatement(args);
    }
    else if (match(TOKEN_WHILE))
    {
        whileStatement(args);
    }
    else if (match(TOKEN_LEFT_BRACE))
    {
        beginScope();
        block();
        endScope();
    }
    else
    {
        expressionStatement(args);
    }
}

void Compiler::printStatement(const ParseFnArgs &args)
{
    expression(args);
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

void Compiler::forStatement(const ParseFnArgs &args)
{
    beginScope();
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for.");
    if (match(TOKEN_SEMICOLON))
    {
        // no initializer.
    }
    else if (match(TOKEN_VAR))
    {
        varDeclarationStatement(args);
    }
    else
    {
        expressionStatement(args);
    }

    int loopStart = currentChunk().size();
    int exitJump = -1;
    if (!match(TOKEN_SEMICOLON))
    {
        expression(args);
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

        exitJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP); // condition
    }

    if (!match(TOKEN_RIGHT_PAREN))
    {
        int bodyJump = emitJump(OP_JUMP);
        int incrementStart = currentChunk().size();
        expression(args);
        emitByte(OP_POP);
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        emitLoop(loopStart);
        loopStart = incrementStart;
        patchJump(bodyJump);
    }

    statement(args);
    emitLoop(loopStart);

    if (exitJump != -1)
    {
        patchJump(exitJump);
        emitByte(OP_POP); // condition
    }

    endScope();
}

void Compiler::ifStatement(const ParseFnArgs &args)
{
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression(args);
    consume(TOKEN_RIGHT_PAREN, "Exprect ')' after condition.");

    size_t thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement(args);
    size_t elseJump = emitJump(OP_JUMP);
    patchJump(thenJump);
    emitByte(OP_POP);
    if (match(TOKEN_ELSE))
        statement(args);
    patchJump(elseJump);
}

void Compiler::whileStatement(const ParseFnArgs &args)
{
    const auto loopStart = currentChunk().size();
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression(args);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    const auto exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement(args);
    emitLoop(loopStart);

    patchJump(exitJump);
    emitByte(OP_POP);
}

void Compiler::expressionStatement(const ParseFnArgs &args)
{
    expression(args);
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

void Compiler::expression(const ParseFnArgs &args)
{
    parsePrecedence(PREC_ASSIGNMENT);
}

void Compiler::number(const ParseFnArgs &args)
{
    double value = std::strtod(previous.lexeme().c_str(), nullptr);
    emitConstant(Value(value));
}

void Compiler::grouping(const ParseFnArgs &args)
{
    expression(args);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary(const ParseFnArgs &args)
{
    const auto operatorType = previous.type;

    // Compile the operand.
    parsePrecedence(PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType)
    {
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;

    default: // Unreachable.
        return;
    }
}

void Compiler::binary(const ParseFnArgs &args)
{
    const auto operatorType = previous.type;
    const auto rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule.precedence + 1));

    switch (operatorType)
    {
    case TOKEN_BANG_EQUAL:
        emitBytes(OP_EQUAL, OP_NOT);
        break;
    case TOKEN_EQUAL_EQUAL:
        emitByte(OP_EQUAL);
        break;
    case TOKEN_GREATER:
        emitByte(OP_GREATER);
        break;
    case TOKEN_GREATER_EQUAL:
        emitBytes(OP_LESS, OP_NOT);
        break;
    case TOKEN_LESS:
        emitByte(OP_LESS);
        break;
    case TOKEN_LESS_EQUAL:
        emitBytes(OP_GREATER, OP_NOT);
        break;
    case TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    default: // Unreachable.
        break;
    }
}

void Compiler::literal(const ParseFnArgs &args)
{
    switch (previous.type)
    {
    case TOKEN_FALSE:
        emitByte(OP_FALSE);
        break;
    case TOKEN_NIL:
        emitByte(OP_NIL);
        break;
    case TOKEN_TRUE:
        emitByte(OP_TRUE);
        break;
    default: // Unreachable.
        return;
    }
}

void Compiler::string(const ParseFnArgs &args)
{
    emitConstant(Value(std::string(previous.start + 1, previous.start + previous.length - 1)));
}

void Compiler::variable(const ParseFnArgs &args)
{
    namedVariable(previous, args.canAssign);
}

void Compiler::logicalAnd(const ParseFnArgs &args)
{
    const auto endJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    parsePrecedence(PREC_AND);
    patchJump(endJump);
}

void Compiler::logicalOr(const ParseFnArgs &args)
{
    const auto elseJump = emitJump(OP_JUMP_IF_FALSE);
    const auto endJump = emitJump(OP_JUMP);
    patchJump(elseJump);
    emitByte(OP_POP);
    parsePrecedence(PREC_OR);
    patchJump(endJump);
}

void Compiler::parsePrecedence(Precedence precedence)
{
    advance();
    const auto prefixRule = getRule(previous.type).prefix;
    if (prefixRule == nullptr)
    {
        error("Expect expression.");
        return;
    }

    ParseFnArgs args;
    args.canAssign = precedence <= PREC_ASSIGNMENT;
    std::invoke(prefixRule, *this, args);

    while (precedence <= getRule(current.type).precedence)
    {
        advance();
        const auto infixRule = getRule(previous.type).infix;
        std::invoke(infixRule, *this, args);
    }

    if (args.canAssign && match(TOKEN_EQUAL))
    {
        error("Invalid assignment target.");
    }
}

size_t Compiler::parseVariable(const std::string &errorMessage)
{
    consume(TOKEN_IDENTIFIER, errorMessage);

    declareVariable();
    if (scopeDepth > 0)
        return 0;

    return identifierConstant(previous);
}

size_t Compiler::identifierConstant(const Token &name)
{
    return makeConstant(Value(name.lexeme()));
}

void Compiler::namedVariable(const Token &name, bool canAssign)
{
    uint8_t getOp, setOp;
    int offset = resolveLocal(name);
    if (offset != -1)
    {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    }
    else
    {
        offset = identifierConstant(name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign && match(TOKEN_EQUAL))
    {
        ParseFnArgs args;
        args.canAssign = true;
        expression(args);
        emitBytes(setOp, offset);
    }
    else
    {
        emitBytes(getOp, offset);
    }
}

ParseRule &Compiler::getRule(TokenType tokenType)
{
    return PARSE_RULES[tokenType];
}

void Compiler::defineVariable(size_t global)
{
    if (scopeDepth > 0)
    {
        markInitialized();
        return;
    }
    emitBytes(OP_DEFINE_GLOBAL, global);
}

void Compiler::declareVariable()
{
    if (scopeDepth == 0)
        return;
    const auto &name = previous;

    auto it = locals.rbegin();
    while (it != locals.rend())
    {
        if (it->depth != -1 && it->depth < scopeDepth)
            break;

        const auto declaringName = name.lexeme();
        if (declaringName == it->name.lexeme())
        {
            std::ostringstream ss;
            ss << "Already a variable named '" << declaringName << "' in this scope.";
            error(ss.str());
        }

        it++;
    }

    addLocal(name);
}

void Compiler::markInitialized()
{
    if (scopeDepth == 0)
        return;
    locals[locals.size() - 1].depth = scopeDepth;
}

void Compiler::addLocal(const Token &name)
{
    Local local;
    local.name = name;
    local.depth = -1;
    locals.push_back(local);
}

int Compiler::resolveLocal(const Token &name)
{
    auto localName = name.lexeme();
    for (int i = locals.size() - 1; i >= 0; i--)
    {
        if (localName == locals[i].name.lexeme())
        {
            if (locals[i].depth == -1)
            {
                std::ostringstream ss;
                ss << "Can't read local variable '" << localName << "' in its own initialized.";
                error(ss.str());
            }
            return i;
        }
    }
    return -1;
}

void Compiler::beginScope()
{
    scopeDepth++;
}

void Compiler::block()
{
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF))
    {
        ParseFnArgs args{true};
        declaration(args);
    }

    consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

void Compiler::compileDefinition(FunctionType type, const std::string &name)
{
    std::cout << "Defintion: " << name << std::endl;
    Compiler compiler(*this);
    auto newFunction = compiler.compileFunction(*this, type, name);
    previous = compiler.previous;
    current = compiler.current;
    emitBytes(OP_CONSTANT, makeConstant(Value(newFunction)));
}

void Compiler::endScope()
{
    scopeDepth--;

    while (locals.size() > 0 && locals[locals.size() - 1].depth > scopeDepth)
    {
        emitByte(OP_POP);
        locals.pop_back();
    }
}

void Compiler::emitByte(uint8_t byte)
{
    currentChunk().write(byte, previous.line);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2)
{
    currentChunk().write(byte1);
    currentChunk().write(byte2);
}

size_t Compiler::makeConstant(Value value)
{
    const auto constantOffset = currentChunk().writeConstant(value);
    // TODO: support OP_CONSTANT_LONG opcode.
    if (constantOffset > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }
    return constantOffset;
}

void Compiler::emitEpilogue()
{
    emitReturn();
}

void Compiler::emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

void Compiler::emitReturn()
{
    emitByte(OP_RETURN);
}

size_t Compiler::emitJump(uint8_t instruction)
{
    emitByte(instruction);
    emitByte(0xff);
    emitByte(0xff);
    return currentChunk().size() - 2;
}

void Compiler::patchJump(size_t offset)
{
    size_t jump = currentChunk().size() - offset - 2;
    if (jump > UINT16_MAX)
    {
        error("Too much code to jump over.");
    }
    uint8_t *code = currentChunk().data();
    code[offset] = (jump >> 8) & 0xFF;
    code[offset + 1] = jump & 0xFF;
}

void Compiler::emitLoop(size_t loopStart)
{
    emitByte(OP_LOOP);
    const auto offset = currentChunk().size() - loopStart + 2;
    if (offset > UINT16_MAX)
        error("Loop body too large.");
    emitByte((offset >> 8) & 0xff);
    emitByte(offset & 0xff);
}

void Compiler::consume(TokenType tokenType, const std::string &errorMessage)
{
    if (current.type == tokenType)
    {
        advance();
        return;
    }
    errorAtCurrent(errorMessage);
}

void Compiler::synchronize()
{
    panicMode = false;

    while (current.type != TOKEN_EOF)
    {
        if (previous.type == TOKEN_SEMICOLON)
            return;
        switch (current.type)
        {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;
        default:
            break;
        }

        advance();
    }
}

void Compiler::error(const std::string &message)
{
    errorAt(previous, message);
}

void Compiler::errorAtCurrent(const std::string &message)
{
    errorAt(current, message);
}

void Compiler::errorAt(Token &token, const std::string &message)
{
    if (panicMode)
        return;
    panicMode = true;
    std::ostringstream ss;
    ss << "[line " << token.line << "] Error";
    if (token.type == TOKEN_EOF)
    {
        ss << " at end";
    }
    else if (token.type == TOKEN_ERROR)
    {
        // omit
    }
    else
    {
        ss << " at " << token.length;
    }

    ss << ": " << message;
    errors.push_back(ss.str());
}