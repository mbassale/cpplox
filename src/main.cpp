#include <gflags/gflags.h>

#include "chunk.h"
#include "common.h"
#include "compiler_v2.h"
#include "ctime"
#include "debug.h"
#include "parser.h"
#include "scanner.h"
#include "vm.h"

#define EXIT_CMDLINE_HELP 64

DEFINE_bool(trace, false, "Enable all tracing");

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
      const auto compilerConfig = buildCompilerConfig();
      cpplox::CompilerV2 compiler(compilerConfig);
      const auto script = compiler.compile("main", program);
      if (compiler.hasErrors()) {
        for (const std::string &error : compiler.getErrors()) {
          std::cerr << error << std::endl;
        }
        return InterpretResult::INTERPRET_COMPILE_ERROR;
      }
      if (FLAGS_trace) {
        Disassembler disassembler(script->getChunk());
        const auto instructions = disassembler.disassemble();
        OpCodePrinter printer(script->getChunk(), instructions);
        printer.print();
      }

      if (FLAGS_trace) {
        std::cout << "== execution ==" << std::endl;
      }
      return vm.interpret(script);
    } catch (VMRuntimeError &err) {
      std::cerr << "RuntimeError: " << err.what() << std::endl;
      return InterpretResult::INTERPRET_RUNTIME_ERROR;
    } catch (cpplox::CompilerError &err) {
      std::cerr << "CompilerError: " << err.what() << std::endl;
      return InterpretResult::INTERPRET_COMPILE_ERROR;
    }
  }

 private:
  cpplox::CompilerConfig buildCompilerConfig() {
    cpplox::CompilerConfig compilerConfig;
    compilerConfig.disassembleInstructions = FLAGS_trace;
    compilerConfig.dumpTokens = FLAGS_trace;
    return compilerConfig;
  }
};

int main(int argc, char *argv[]) {
  Driver driver;

  std::ostringstream usage_ss;
  usage_ss << argv[0] << " [script-path]";
  gflags::SetUsageMessage(usage_ss.str());
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  if (argc == 1) {
    driver.repl();
  } else if (argc == 2) {
    driver.runFile(argv[1]);
  } else {
    exit(EXIT_CMDLINE_HELP);
  }

  return EXIT_SUCCESS;
}