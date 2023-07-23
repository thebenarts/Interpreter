#include "Lexer.h"
#include "Token.h"
#include "Utility.h"
#include "AbstractSyntaxTree.h"
#include "Parser.h"
#include "Logger.h"
#include <ranges>
#include <algorithm>
#include <vector>

int main()
{
    std::string input;
    if(std::getline(std::cin,input))
    {
        interpreter::LexerUniquePtr lexer{ std::make_unique<interpreter::Lexer>(input) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };
        interpreter::LOG_MESSAGE(program.get());
    }

    std::ofstream o{ "HelloWorld.txt" };
    return 0;
}
