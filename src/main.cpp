#include "Lexer.h"
#include "Token.h"
#include "Utility.h"

// TestData 
//`let five = 5;
//let ten = 10;
//let add = fn(x, y) {
//x + y;
//};
//let result = add(five, ten);

int main()
{
    std::string testInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/testData.txt") };

    std::vector<interpreter::Token> expected
    {
        {interpreter::TokenType::LET, "let"},
        {interpreter::TokenType::IDENT, "five"},
        {interpreter::TokenType::ASSIGN, "="},
        {interpreter::TokenType::INT, "5"},
        {interpreter::TokenType::SEMICOLON, ";"},
        {interpreter::TokenType::LET, "let"},
        {interpreter::TokenType::IDENT, "ten"},
        {interpreter::TokenType::ASSIGN, "="},
        {interpreter::TokenType::INT, "10"},
        {interpreter::TokenType::SEMICOLON, ";"},
        {interpreter::TokenType::LET, "let"},
        {interpreter::TokenType::IDENT, "add"},
        {interpreter::TokenType::ASSIGN, "="},
        {interpreter::TokenType::FUNCTION, "fn"},
        {interpreter::TokenType::LPAREN, "("},
        {interpreter::TokenType::IDENT, "x"},
        {interpreter::TokenType::COMMA, ","},
        {interpreter::TokenType::IDENT, "y"},
        {interpreter::TokenType::RPAREN, ")"},
        {interpreter::TokenType::LBRACE, "{"},
        {interpreter::TokenType::IDENT, "x"},
        {interpreter::TokenType::PLUS, "+"},
        {interpreter::TokenType::IDENT, "y"},
        {interpreter::TokenType::SEMICOLON, ";"},
        {interpreter::TokenType::RBRACE, "}"},
        {interpreter::TokenType::SEMICOLON, ";"},
        {interpreter::TokenType::LET, "let"},
        {interpreter::TokenType::IDENT, "result"},
        {interpreter::TokenType::ASSIGN, "="},
        {interpreter::TokenType::IDENT, "add"},
        {interpreter::TokenType::LPAREN, "("},
        {interpreter::TokenType::IDENT, "five"},
        {interpreter::TokenType::COMMA, ","},
        {interpreter::TokenType::IDENT, "ten"},
        {interpreter::TokenType::RPAREN, ")"},
        {interpreter::TokenType::SEMICOLON, ";"},
        {interpreter::TokenType::BANG, "!"},
		{interpreter::TokenType::MINUS, "-"},
		{interpreter::TokenType::SLASH, "/"},
		{interpreter::TokenType::ASTERISK, "*"},
		{interpreter::TokenType::INT, "5"},
		{interpreter::TokenType::SEMICOLON, ";"},
		{interpreter::TokenType::INT, "5"},
		{interpreter::TokenType::LT, "<"},
		{interpreter::TokenType::INT, "10"},
		{interpreter::TokenType::GT, ">"},
		{interpreter::TokenType::INT, "5"},
		{interpreter::TokenType::SEMICOLON, ";"},
		{interpreter::TokenType::IF, "if"},
		{interpreter::TokenType::LPAREN, "("},
		{interpreter::TokenType::INT, "5"},
		{interpreter::TokenType::LT, "<"},
		{interpreter::TokenType::INT, "10"},
		{interpreter::TokenType::RPAREN, ")"},
		{interpreter::TokenType::LBRACE, "{"},
		{interpreter::TokenType::RETURN, "return"},
		{interpreter::TokenType::TRUE, "true"},
		{interpreter::TokenType::SEMICOLON, ";"},
		{interpreter::TokenType::RBRACE, "}"},
		{interpreter::TokenType::ELSE, "else"},
		{interpreter::TokenType::LBRACE, "{"},
		{interpreter::TokenType::RETURN, "return"},
		{interpreter::TokenType::FALSE, "false"},
		{interpreter::TokenType::SEMICOLON, ";"},
		{interpreter::TokenType::RBRACE, "}"},
		{interpreter::TokenType::INT, "10"},
		{interpreter::TokenType::EQ, "=="},
		{interpreter::TokenType::INT, "10"},
		{interpreter::TokenType::SEMICOLON, ";"},
		{interpreter::TokenType::INT, "10"},
		{interpreter::TokenType::NOT_EQ, "!="},
		{interpreter::TokenType::INT, "9"},
		{interpreter::TokenType::SEMICOLON, ";"},
        {interpreter::TokenType::ENDF, ""}
    };

    interpreter::Lexer lexer{ testInput };

    std::vector<interpreter::Token> results{ lexer.Tokenize() };
    std::cout << '\n';
    if(interpreter::utility::CompareTokens(expected, results))
    {
        std::cout << '\n' << "SUCCESS";
    }
    else
    {
        std::cout << '\n' << "FAIL";
    }

    return 0;
}
