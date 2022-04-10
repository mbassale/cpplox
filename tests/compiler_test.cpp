#include <gtest/gtest.h>
#include "common.h"
#include "compiler.h"

struct CompilerTestData
{
    const char *name;
    const char *source;

    CompilerTestData(const char *name, const char *source) : name(name), source(source) {}
};

class CompilerTest : public ::testing::Test
{
};

TEST_F(CompilerTest, CompileAssertions)
{
    std::array<CompilerTestData, 1> testCases = {
        CompilerTestData("PrintStmt", "print 1+2*3/4;"),
    };
    for (const auto &testCase : testCases)
    {
        CompilerConfig config;
        config.dumpTokens = false;
        config.disassembleInstructions = true;
        Compiler compiler(config);
        auto script = compiler.compile(testCase.name, testCase.source);
        ASSERT_EQ(script->getName(), testCase.name);
        ASSERT_GE(script->getChunk().size(), 0);
    }
}