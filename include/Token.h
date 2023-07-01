#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include "ForwardDeclares.h"

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
        TRUE,
        FALSE,
        IF,
        ELSE,
        RETURN,

    };

    struct Token
    {
        TokenType mType;
        std::variant<std::string, Number> mLiteral;
        int32_t mLineNumber;
        CharacterRange mCharacterRange[2];
    };
    std::ostringstream& operator<<(std::ostringstream& out, const Token& token);

    static std::unordered_map<std::string, TokenType> sKeywordsMap
    {
        {"fn",TokenType::FUNCTION},
        {"let", TokenType::LET},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
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
        {TokenType::TRUE, "TRUE"},
        {TokenType::FALSE, "FALSE"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::RETURN, "RETURN"}
    };

    static std::unordered_set<TokenType> sStringTokens
    {
        {TokenType::IDENT},
        {TokenType::ASSIGN},
        {TokenType::PLUS},
        {TokenType::MINUS},
        {TokenType::BANG},
        {TokenType::ASTERISK},
        {TokenType::SLASH},
        {TokenType::LT},
        {TokenType::GT},
        {TokenType::EQ},
        {TokenType::NOT_EQ},
        {TokenType::COMMA},
        {TokenType::SEMICOLON},
        {TokenType::LPAREN},
        {TokenType::RPAREN},
        {TokenType::LBRACE},
        {TokenType::RBRACE},
        {TokenType::FUNCTION},
        {TokenType::LET},
        {TokenType::IF},
        {TokenType::ELSE},
        {TokenType::RETURN}
    };

    static std::unordered_set<TokenType> sNumberTokens
    {
        {TokenType::INT}
    };

    static std::unordered_set<TokenType> sBooleanTokens
    {
        {TokenType::TRUE},
        {TokenType::FALSE}
    };

}