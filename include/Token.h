#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

namespace interpreter
{
    enum class TokenType
    {
        ILLEGAL,
        ENDF,   // EOF

        // Identifiers + literals
        IDENT,
        INT,

        // Operators
        ASSIGN,     // "="
        PLUS,       // "+"
        MINUS,      // "-"
        BANG,       // "!"
        ASTERISK,   // "*"
        SLASH,      // "/"

        LT,         // "<"
        GT,         // ">"

        EQ,         // "=="
        NOT_EQ,     // "!="

        // Delimeters
        COMMA,      // ","
        SEMICOLON,  // ";"

        LPAREN,     // "("
        RPAREN,     // ")"
        LBRACE,     // "{"
        RBRACE,     // "}"

        // Keywords
        FUNCTION,
        LET,
        True,
        False,
        IF,
        ELSE,
        RETURN,

    };

    struct Token
    {
        TokenType mType;
        std::string mLiteral;
    };

    static std::unordered_map<std::string, TokenType> sKeywordsMap
    {
        {"fn",TokenType::FUNCTION},
        {"let", TokenType::LET},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"if", TokenType::IF},
        {"else",TokenType::ELSE},
        {"return", TokenType::RETURN}
    };

    static std::unordered_map<TokenType, std::string> sTokenTypeToStringMap
    {
        {TokenType::ILLEGAL,"ILLEGAL"},
        {TokenType::ENDF, "EOF"},
        {TokenType::IDENT, "IDENTIFIER"},
        {TokenType::INT, "INT"},
        {TokenType::ASSIGN, "ASSIGN"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::BANG, "BANG"},
        {TokenType::ASTERISK, "ASTERISK"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::LT, "LT"},
        {TokenType::GT, "GT"},
        {TokenType::EQ, "EQ"},
        {TokenType::NOT_EQ, "NOT_EQ"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::LPAREN, "LPAREN"},
        {TokenType::RPAREN, "RPAREN"},
        {TokenType::LBRACE, "LBRACE"},
        {TokenType::RBRACE, "RBRACE"},
        {TokenType::FUNCTION, "FUNCTION"},
        {TokenType::LET, "LET"},
        {TokenType::True, "TRUE"},
        {TokenType::False, "FALSE"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::RETURN, "RETURN"}
    };
          
}