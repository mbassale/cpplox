#include <gflags/gflags.h>

#include "astbuilder.h"
#include "common.h"
#include "evaluator.h"
#include "lexer.h"
#include "parser.h"
#include "settings.h"
#include "token.h"

#define EXIT_CMDLINE_HELP 64

using Parser::JSParser;

DEFINE_bool(debug, false, "Enable debugging");

void fixNewLineAtEOF(std::string &source) {
  if (source.length() > 0 && source[source.length() - 1] != '\n') {
    source.push_back('\n');
  }
}

class Driver {
 private:
  Evaluator evaluator;

 public:
  Driver() {
    if (FLAGS_debug) {
      Settings::getInstance()->debugMode = true;
    }
  }

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
    auto source = sstr.str();
    fixNewLineAtEOF(source);
    interpret(source);
  }

  bool interpret(const std::string &source) {
    try {
      LOG(INFO) << "======== PARSING START ========";
      ASTBuilderImpl builder;
      std::stringstream ss(source);
      JSLexer lexer(&ss);
      JSParser parser(builder, lexer);
      parser.parse();
      auto program = builder.getProgram();
      LOG(INFO) << "Program: " << (program ? program->toString() : "nullptr");
      LOG(INFO) << "======== PARSING END ========";
      if (program == nullptr) {
        return false;
      }
      LOG(INFO) << "======== EVALUATION START ========";
      auto value = evaluator.eval(program);
      LOG(INFO) << "Result: " << (value ? value->toString() : "nullptr");
      LOG(INFO) << "======== EVALUATION END ========";
      return true;
    } catch (std::exception &ex) {
      LOG(ERROR) << "RuntimeError: " << ex.what();
      return false;
    }
  }
};

int main(int argc, char *argv[]) {
  std::ostringstream usage_ss;
  usage_ss << argv[0] << " [script-path]";
  gflags::SetUsageMessage(usage_ss.str());
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  Driver driver;
  if (argc == 1) {
    driver.repl();
  } else if (argc == 2) {
    driver.runFile(argv[1]);
  } else {
    exit(EXIT_CMDLINE_HELP);
  }

  return EXIT_SUCCESS;
}