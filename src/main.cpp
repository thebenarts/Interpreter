#include "Lexer.h"
#include "Token.h"
#include "Utility.h"
#include "AbstractSyntaxTree.h"
#include "Parser.h"


int main()
{
  //  std::string lexerInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/testData.txt") };
  //  std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/parserTestData.txt") };

  //  interpreter::LexerUniquePtr lexer{ std::make_unique<interpreter::Lexer>(lexerInput) };

  //  std::vector<interpreter::Token> results{ lexer->GetTokens() };
  //  std::cout << '\n';
  //  if(interpreter::utility::CompareTokens(expected, results))
  //  {
  //      std::cout << '\n' << "SUCCESS";
  //  }
  //  else
  //  {
  //      std::cout << '\n' << "FAIL";
  //  }
  //  std::cout << std::endl;

  //  interpreter::LexerUniquePtr lexer2{ std::make_unique<interpreter::Lexer>(parserInput) };
  //  interpreter::Parser parser{std::move(lexer2)};
  //  interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

  //  if (!program)
  //  {
  //      std::cout << "ERROR: PARSER RETURNED NULL" << std::endl;
  //      return 1;
  //  }

  //  std::vector<std::string> expectedIdentifiers{ "x","y","foobar" };
  //  if (program->mStatements.size() != 3)
  //  {
  //      std::cout << "ERROR: PROGRAM DOESN'T CONTAIN 3 STATEMENTS" << std::endl;
  //  }

  //  for (int i = 0; i != 3; i++)
  //  {
  //      interpreter::utility::TestLetStatement(program->mStatements[i].get(), expectedIdentifiers[i]);
  //  }

    return 0;
}
