#include "common.h"
#include "chunk.h"
#include "vm.h"

#define EXIT_CMDLINE_HELP 64

class Driver
{
private:
    VM vm;

public:
    Driver() : vm()
    {
    }

    void repl()
    {
        for (;;)
        {
            std::cout << "> ";
            std::string line;
            std::getline(std::cin, line);
            if (line == "quit")
            {
                break;
            }
            interpret(line);
        }
    }

    void runFile(const char *path)
    {
        std::ifstream file(path);
        if (file.bad())
        {
            throw std::runtime_error("Cannot open file.");
        }
        std::ostringstream sstr;
        sstr << file.rdbuf();
        interpret(sstr.str());
    }

    InterpretResult interpret(const std::string &source)
    {
        Chunk chunk("<main>");
        chunk.disassemble();

        std::cout << "== execution ==" << std::endl;
        return vm.interpret(chunk);
    }
};

int main(int argc, char *argv[])
{
    Driver driver;

    if (argc == 1)
    {
        driver.repl();
    }
    else if (argc == 2)
    {
        driver.runFile(argv[1]);
    }
    else
    {
        std::cerr << "Usage: cpplox [path]\n";
        exit(EXIT_CMDLINE_HELP);
    }

    return EXIT_SUCCESS;
}