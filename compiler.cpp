#include "compiler.h"

void Compiler::compile(const std::string &source)
{
    Scanner scanner(source);

    for (;;)
    {
        Token token = scanner.next();
        std::cout << std::setfill('0') << std::setw(4) << token.line << " " << token.type << " " << std::endl;
        if (token.type == TOKEN_EOF)
            break;
    }
}