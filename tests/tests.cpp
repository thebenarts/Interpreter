#include "Utility.h"
#include "Parser.h"
#include "AbstractSyntaxTree.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// TestData 
//`let five = 5;
//let ten = 10;
//let add = fn(x, y) {
//x + y;
//};
//let result = add(five, ten);

namespace interpreter
{
    TEST_CASE("LEXER TEST")
    {
        std::string lexerInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/testData.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<interpreter::Lexer>(lexerInput) };
        std::vector<interpreter::Token> expected
        {
            {TokenType::LET, "let"},
            {TokenType::IDENT, "five"},
            {TokenType::ASSIGN, "="},
            {TokenType::INT, "5"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::LET, "let"},
            {TokenType::IDENT, "ten"},
            {TokenType::ASSIGN, "="},
            {TokenType::INT, "10"},
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
            {TokenType::INT, "5"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::INT, "5"},
            {TokenType::LT, "<"},
            {TokenType::INT, "10"},
            {TokenType::GT, ">"},
            {TokenType::INT, "5"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::IF, "if"},
            {TokenType::LPAREN, "("},
            {TokenType::INT, "5"},
            {TokenType::LT, "<"},
            {TokenType::INT, "10"},
            {TokenType::RPAREN, ")"},
            {TokenType::LBRACE, "{"},
            {TokenType::RETURN, "return"},
		    {TokenType::True, "true"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::ELSE, "else"},
            {TokenType::LBRACE, "{"},
            {TokenType::RETURN, "return"},
		    {TokenType::False, "false"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::INT, "10"},
            {TokenType::EQ, "=="},
            {TokenType::INT, "10"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::INT, "10"},
            {TokenType::NOT_EQ, "!="},
            {TokenType::INT, "9"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::ENDF, ""}
        };
        std::vector<Token> results{ lexer->GetTokenCopies() };

        REQUIRE(expected.size() == results.size());

        for (int i = 0; i < std::min(results.size(), expected.size()); i++)
        {
            REQUIRE(expected[i].mType == results[i].mType);
            REQUIRE(expected[i].mLiteral == results[i].mLiteral);
        }
    }

    TEST_CASE("PARSER")
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
            REQUIRE(letStatement->mIdentifier.mLiteral == expectedIdentifiers[i]);
        }
    }
}
