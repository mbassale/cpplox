#include "chunk.h"
#include "common.h"
#include "compiler_v2.h"
#include "ctime"
#include "debug.h"
#include "parser.h"
#include "scanner.h"
#include "vm.h"

#define EXIT_CMDLINE_HELP 64

static Value clockNative(int argCount, Value *args) {
  return Value((double)clock() / CLOCKS_PER_SEC);
}

class Driver {
 private:
  VM vm;

 public:
  Driver() : vm() { vm.defineNative("clock", clockNative); }

  void repl() {
    std::cout << "CppLox v" << CPPLOX_VERSION_MAJOR << "."
              << CPPLOX_VERSION_MINOR << " - © 2022 Marco Bassaletti."
              << std::endl;
    std::string line;
    for (;;) {
      std::cout << "> ";
      line.clear();
      const auto &retVal = std::getline(std::cin, line);
      if (retVal.eof() || retVal.bad() || line == "quit") {
        break;
      }
      interpret(line);
    }
  }

  void runFile(const char *path) {
    std::ifstream file(path);
    if (file.bad()) {
      throw std::runtime_error("Cannot open file.");
    }
    std::ostringstream sstr;
    sstr << file.rdbuf();
    interpret(sstr.str());
  }

  InterpretResult interpret(const std::string &source) {
    try {
      Scanner scanner(source);
      cpplox::Parser parser(scanner);
      const auto program = parser.parse();
      if (parser.hasErrors()) {
        for (const cpplox::ParserException &error : parser.getErrors()) {
          std::cerr << error.what() << std::endl;
        }
        return InterpretResult::INTERPRET_PARSING_ERROR;
      }
      cpplox::CompilerV2 compiler;
      const auto script = compiler.compile("main", program);
      if (compiler.hasErrors()) {
        for (const std::string &error : compiler.getErrors()) {
          std::cerr << error << std::endl;
        }
        return InterpretResult::INTERPRET_COMPILE_ERROR;
      }
      Disassembler disassembler(script->getChunk());
      const auto instructions = disassembler.disassemble();
      OpCodePrinter printer(script->getChunk(), instructions);
      printer.print();

      std::cout << "== execution ==" << std::endl;
      return vm.interpret(script);
    } catch (VMRuntimeError &err) {
      std::cerr << "RuntimeError: " << err.what() << std::endl;
      return InterpretResult::INTERPRET_RUNTIME_ERROR;
    } catch (CompilerError &err) {
      std::cerr << "CompilerError: " << err.what() << std::endl;
      return InterpretResult::INTERPRET_COMPILE_ERROR;
    }
  }
};

int main(int argc, char *argv[]) {
  Driver driver;

  if (argc == 1) {
    driver.repl();
  } else if (argc == 2) {
    driver.runFile(argv[1]);
  } else {
    std::cerr << "Usage: cpplox [path]\n";
    exit(EXIT_CMDLINE_HELP);
  }

  return EXIT_SUCCESS;
}