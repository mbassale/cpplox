#include <gtest/gtest.h>
#include "common.h"
#include "compiler.h"
#include "debug.h"

struct CompilerTestData
{
    const char *name;
    const char *source;
    std::vector<VMInstr> expectedInstructions;

    CompilerTestData(const char *name, const char *source, const std::vector<VMInstr> &instructions) : name(name), source(source), expectedInstructions(instructions) {}
};

class CompilerTest : public ::testing::Test
{
};

TEST_F(CompilerTest, CompileAssertions)
{
    std::vector<CompilerTestData> testCases = {
        CompilerTestData(
            "PrintStmt",
            "print 1+2*3/4;",
            std::vector<VMInstr>{
                VMInstr(OP_CONSTANT),
                VMInstr(OP_CONSTANT),
                VMInstr(OP_CONSTANT),
                VMInstr(OP_MULTIPLY),
                VMInstr(OP_CONSTANT),
                VMInstr(OP_DIVIDE),
                VMInstr(OP_ADD),
                VMInstr(OP_PRINT),
                VMInstr(OP_NIL),
                VMInstr(OP_RETURN)}),
        CompilerTestData(
            "VarStmt",
            "var test=1+2;",
            std::vector<VMInstr>{
                VMInstr::makeConstant(0),
                VMInstr::makeConstant(1),
                VMInstr(OP_ADD),
                VMInstr::makeDefineGlobal(0),
                VMInstr(OP_NIL),
                VMInstr(OP_RETURN)}),
        CompilerTestData(
            "IfStmt",
            "if(1<1+1){print true;}else{print false;}",
            std::vector<VMInstr>{
                VMInstr::makeConstant(0),
                VMInstr::makeConstant(1),
                VMInstr::makeConstant(2),
                VMInstr(OP_ADD),
                VMInstr(OP_LESS),
                VMInstr(OP_JUMP_IF_FALSE),
                VMInstr(OP_POP),
                VMInstr(OP_TRUE),
                VMInstr(OP_PRINT),
                VMInstr(OP_JUMP),
                VMInstr(OP_POP),
                VMInstr(OP_FALSE),
                VMInstr(OP_PRINT),
                VMInstr(OP_NIL),
                VMInstr(OP_RETURN)}),
        CompilerTestData(
            "WhileStmt",
            "while(true){print true;}",
            std::vector<VMInstr>{
                VMInstr(OP_TRUE),
                VMInstr(OP_JUMP_IF_FALSE),
                VMInstr(OP_POP),
                VMInstr(OP_TRUE),
                VMInstr(OP_PRINT),
                VMInstr(OP_LOOP),
                VMInstr(OP_POP),
                VMInstr(OP_NIL),
                VMInstr(OP_RETURN)}),
        CompilerTestData(
            "ForStmt",
            "for(var i=0;i<10;i=i+1){print i;}",
            std::vector<VMInstr>{
                VMInstr::makeConstant(0),
                VMInstr(OP_GET_LOCAL),
                VMInstr::makeConstant(1),
                VMInstr(OP_LESS),
                VMInstr(OP_JUMP_IF_FALSE),
                VMInstr(OP_POP),
                VMInstr(OP_JUMP),
                VMInstr(OP_GET_LOCAL),
                VMInstr::makeConstant(2),
                VMInstr(OP_ADD),
                VMInstr(OP_SET_LOCAL),
                VMInstr(OP_POP),
                VMInstr(OP_LOOP),
                VMInstr(OP_GET_LOCAL),
                VMInstr(OP_PRINT),
                VMInstr(OP_LOOP),
                VMInstr(OP_POP),
                VMInstr(OP_POP),
                VMInstr(OP_NIL),
                VMInstr(OP_RETURN)})};

    for (const auto &testCase : testCases)
    {
        CompilerConfig config;
        config.dumpTokens = false;
        config.disassembleInstructions = true;
        Compiler compiler(config);
        auto script = compiler.compile(testCase.name, testCase.source);
        ASSERT_EQ(script->getName(), testCase.name);
        ASSERT_GE(script->getChunk().size(), 0);

        Disassembler disassembler(script->getChunk());
        const auto actualInstructions = disassembler.disassemble();
        OpCodePrinter opCodePrinter(script->getChunk(), actualInstructions);
        opCodePrinter.print();
        ASSERT_EQ(actualInstructions.size(), testCase.expectedInstructions.size());
        ASSERT_TRUE(
            std::equal(
                actualInstructions.begin(),
                actualInstructions.end(),
                testCase.expectedInstructions.begin(),
                [](const VMInstr &actualInstr, const VMInstr &expectedInstr)
                {
                    return actualInstr.opCode == expectedInstr.opCode;
                }));
    }
}