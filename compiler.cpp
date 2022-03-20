#include "compiler.h"

void Compiler::compile(const std::string &source)
{
    Scanner scanner(source);

    for (;;)
    {
        Token token = scanner.next();
        std::cout << token.str() << std::endl;
        if (token.type == TOKEN_EOF)
            break;
    }
}