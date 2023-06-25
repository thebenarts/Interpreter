#include "Utility.h"
#include "Parser.h"
#include "AbstractSyntaxTree.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#undef TRUE
#undef FALSE

namespace interpreter
{
    TEST_CASE("LEXER TEST")
    {
// TestData 
//`let five = 5;
//let ten = 10;
//let add = fn(x, y) {
//x + y;
//};
//let result = add(five, ten);
        std::string lexerInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/lexerTestData.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<interpreter::Lexer>(lexerInput) };
        std::vector<interpreter::Token> expected
        {
            {TokenType::LET, "let"},
            {TokenType::IDENT, "five"},
            {TokenType::ASSIGN, "="},
            {TokenType::INT, 5},
            {TokenType::SEMICOLON, ";"},
            {TokenType::LET, "let"},
            {TokenType::IDENT, "ten"},
            {TokenType::ASSIGN, "="},
            {TokenType::INT, 10},
            {TokenType::SEMICOLON, ";"},
            {TokenType::LET, "let"},
            {TokenType::IDENT, "add"},
            {TokenType::ASSIGN, "="},
            {TokenType::FUNCTION, "fn"},
            {TokenType::LPAREN, "("},
            {TokenType::IDENT, "x"},
            {TokenType::COMMA, ","},
            {TokenType::IDENT, "y"},
            {TokenType::RPAREN, ")"},
            {TokenType::LBRACE, "{"},
            {TokenType::IDENT, "x"},
            {TokenType::PLUS, "+"},
            {TokenType::IDENT, "y"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::LET, "let"},
            {TokenType::IDENT, "result"},
            {TokenType::ASSIGN, "="},
            {TokenType::IDENT, "add"},
            {TokenType::LPAREN, "("},
            {TokenType::IDENT, "five"},
            {TokenType::COMMA, ","},
            {TokenType::IDENT, "ten"},
            {TokenType::RPAREN, ")"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::BANG, "!"},
            {TokenType::MINUS, "-"},
            {TokenType::SLASH, "/"},
            {TokenType::ASTERISK, "*"},
            {TokenType::INT, 5},
            {TokenType::SEMICOLON, ";"},
            {TokenType::INT, 5},
            {TokenType::LT, "<"},
            {TokenType::INT, 10},
            {TokenType::GT, ">"},
            {TokenType::INT, 5},
            {TokenType::SEMICOLON, ";"},
            {TokenType::IF, "if"},
            {TokenType::LPAREN, "("},
            {TokenType::INT, 5},
            {TokenType::LT, "<"},
            {TokenType::INT, 10},
            {TokenType::RPAREN, ")"},
            {TokenType::LBRACE, "{"},
            {TokenType::RETURN, "return"},
		    {TokenType::TRUE, "true"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::ELSE, "else"},
            {TokenType::LBRACE, "{"},
            {TokenType::RETURN, "return"},
		    {TokenType::FALSE, "false"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::INT, 10},
            {TokenType::EQ, "=="},
            {TokenType::INT, 10},
            {TokenType::SEMICOLON, ";"},
            {TokenType::INT, 10},
            {TokenType::NOT_EQ, "!="},
            {TokenType::INT, 9},
            {TokenType::SEMICOLON, ";"},
            {TokenType::ENDF, ""}
        };
        std::vector<Token> results{ lexer->GetTokenCopies() };

        REQUIRE(expected.size() == results.size());

        for (int i = 0; i < std::min(results.size(), expected.size()); i++)
        {
            REQUIRE(expected[i].mType == results[i].mType);
            if (std::holds_alternative<std::string>(results[i].mLiteral))
            {
                REQUIRE(std::get<std::string>(expected[i].mLiteral) == std::get<std::string>(results[i].mLiteral));
            }
            else
            {
                REQUIRE(std::get<int64_t>(expected[i].mLiteral) == std::get<int64_t>(results[i].mLiteral));
            }
        }
    }

    TEST_CASE("PARSER Statement tests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/parserTestData.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program != nullptr);

        std::vector<std::string> expectedIdentifiers{ "x","y","foobar" };
        REQUIRE(program->mStatements.size() == 3);

        for (int i = 0; i != 3; i++)
        {
            ast::LetStatement* letStatement{ dynamic_cast<ast::LetStatement*>(program->mStatements[i].get()) };
            REQUIRE(letStatement != nullptr);

            REQUIRE(letStatement->mToken.mType == TokenType::LET);
            if (std::holds_alternative<std::string>(letStatement->mIdentifier.mLiteral))
            {
                REQUIRE(*std::get_if<std::string>(&letStatement->mIdentifier.mLiteral) == expectedIdentifiers[i]);
            }
        }
    }

    TEST_CASE("IdentExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/identifierExpressionStatementsTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program != nullptr);
        REQUIRE(program->mStatements.size() == 1);
        REQUIRE(program->mStatements[0]->mNodeType == ast::NodeType::ExpressionStatement);
        REQUIRE(program->mStatements[0]->mExpression->mToken.mType == TokenType::IDENT);
        REQUIRE(std::holds_alternative<std::string>(program->mStatements[0]->mExpression->mToken.mLiteral) == true);
        REQUIRE(std::get<std::string>(program->mStatements[0]->mExpression->mToken.mLiteral) == "foobar");
    }

    TEST_CASE("IntegerExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/integerExpressionStatementsTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program != nullptr);
        REQUIRE(program->mStatements.size() == 1);
        REQUIRE(program->mStatements[0]->mNodeType == ast::NodeType::ExpressionStatement);
        REQUIRE(program->mStatements[0]->mExpression->mToken.mType == TokenType::INT);
        REQUIRE(std::holds_alternative<int64_t>(program->mStatements[0]->mExpression->mToken.mLiteral) == true);
        REQUIRE(std::get<int64_t>(program->mStatements[0]->mExpression->mToken.mLiteral) == 5);
    }

    TEST_CASE("PrefixOperatorExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/prefixOperatorExpressionStatementsTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        std::vector<std::vector<Token>> prefixTests
        {
            {{TokenType::BANG,"!"},{TokenType::INT, 5}},
            {{TokenType::MINUS, "-"},{TokenType::INT, 15}}
        };

        REQUIRE(program != nullptr);
        REQUIRE(program->mStatements.size() == 2);
        for (int i = 0; i != 2; i++)
        {
            REQUIRE(program->mStatements[i]->mNodeType == ast::NodeType::ExpressionStatement);
            REQUIRE(program->mStatements[i]->mExpression->mExpressionType == ast::ExpressionType::PrefixExpression);
            const auto prefixExpression{ dynamic_cast<ast::PrefixExpression*>(program->mStatements[i]->mExpression.get()) };
            REQUIRE(prefixExpression);

            REQUIRE(prefixExpression->mToken.mType == prefixTests[i][0].mType);
            REQUIRE(prefixExpression->mRightSideValue->mToken.mType == prefixTests[i][1].mType);
            REQUIRE(utility::CompareTokens(prefixExpression->mRightSideValue->mToken, prefixTests[i][1]));
        }
    }
}
