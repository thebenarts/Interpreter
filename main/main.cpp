#include "Lexer.h"
#include "Token.h"
#include "Utility.h"
#include "AbstractSyntaxTree.h"
#include "Parser.h"
#include "Objects.h"
#include "Evaluator.h"

#include <ranges>
#include <algorithm>
#include <vector>
#include <filesystem>

int main()
{
    std::string input;
    interpreter::Logger::SetLoggerSeverity(interpreter::MessageType::WARNING);
    std::cout << "Current Path is " << std::filesystem::current_path() << '\n';

    while (std::getline(std::cin, input))
    {
        interpreter::LexerUniquePtr lexer{ std::make_unique<interpreter::Lexer>(input) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };
        for (const auto& node : program->mStatements)
        {
            if (node)
            {
                const auto object{ interpreter::Evaluator::Evaluate(node.get()) };
                if (object)
                {
                    interpreter::LOG_MESSAGE(object->Inspect());
                }
            }
        }
        //interpreter::LOG_MESSAGE(program.get());
    }

    return 0;
}
