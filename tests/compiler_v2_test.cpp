#include "compiler_v2.h"

#include <gtest/gtest.h>

#include "common.h"
#include "debug.h"
#include "parser.h"

using namespace cpplox;

struct CompilerV2TestData {
  const char *name;
  const char *source;
  std::vector<VMInstr> expectedInstructions;

  CompilerV2TestData(const char *name, const char *source,
                     const std::vector<VMInstr> &instructions)
      : name(name), source(source), expectedInstructions(instructions) {}
};

class CompilerV2Test : public ::testing::Test {};

TEST_F(CompilerV2Test, CompileAssertions) {
  std::vector<CompilerV2TestData> testCases = {
      CompilerV2TestData(
          "PrintStmt", "print 1+2*3/4;",
          std::vector<VMInstr>{VMInstr(OP_CONSTANT), VMInstr(OP_CONSTANT),
                               VMInstr(OP_CONSTANT), VMInstr(OP_MULTIPLY),
                               VMInstr(OP_CONSTANT), VMInstr(OP_DIVIDE),
                               VMInstr(OP_ADD), VMInstr(OP_PRINT),
                               VMInstr(OP_NIL), VMInstr(OP_RETURN)}),
      CompilerV2TestData(
          "IfStmt", "if(1<1+1){print true;}else{print false;}",
          std::vector<VMInstr>{
              VMInstr::makeConstant(0), VMInstr::makeConstant(1),
              VMInstr::makeConstant(2), VMInstr(OP_ADD), VMInstr(OP_LESS),
              VMInstr(OP_JUMP_IF_FALSE), VMInstr(OP_POP), VMInstr(OP_TRUE),
              VMInstr(OP_PRINT), VMInstr(OP_JUMP), VMInstr(OP_POP),
              VMInstr(OP_FALSE), VMInstr(OP_PRINT), VMInstr(OP_NIL),
              VMInstr(OP_RETURN)}),
  };

  for (const auto &testCase : testCases) {
    CompilerConfig config;
    config.dumpTokens = false;
    config.disassembleInstructions = true;
    CompilerV2 compiler(config);
    Scanner scanner(testCase.source);
    Parser parser(scanner);
    const auto programAst = parser.parse();
    auto script = compiler.compile(testCase.name, programAst);
    ASSERT_EQ(script->getName(), testCase.name);
    ASSERT_GE(script->getChunk().size(), 0);

    Disassembler disassembler(script->getChunk());
    const auto actualInstructions = disassembler.disassemble();
    OpCodePrinter opCodePrinter(script->getChunk(), actualInstructions);
    opCodePrinter.print();
    ASSERT_EQ(actualInstructions.size(), testCase.expectedInstructions.size());
    ASSERT_TRUE(std::equal(
        actualInstructions.begin(), actualInstructions.end(),
        testCase.expectedInstructions.begin(),
        [](const VMInstr &actualInstr, const VMInstr &expectedInstr) {
          return actualInstr.opCode == expectedInstr.opCode;
        }));
  }
}