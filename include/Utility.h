#pragma once

#include "Token.h"
#include "Lexer.h"
#include "ForwardDeclares.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <optional>

#include <fstream>
#include <assert.h>

# define VERIFY(expression) assert(expression); if(expression) 

# define VERIFY_MESSAGE(expression,message) if (!expression) std::cout << message << std::endl;  \
    assert(expression); \
    if(expression)

# define IF_LOG(expression, message) if(!expression) std::cout << message << std::endl; \
    if (expression)

namespace interpreter
{
    namespace utility
    {
        bool IsLetter(char character);
        bool IsDigit(char character);
        int64_t ToNumber(std::string_view literal);

        bool CompareTokens(const Token& left, const Token& right);
        std::string ConvertTokenTypeToString(TokenType tokenType);

        void AssignToToken(Token& token, TokenType tokenType, std::string_view literal, CharacterRange* characterRange);
        void AssignToToken(Token& token, TokenType tokenType, const char literal, CharacterRange* characterRange);
        void AssignToToken(Token& token, TokenType tokenType, int64_t literal, CharacterRange* characterRange);
        TokenType DeriveIdentifierToken(std::string_view literal);
        std::string ReadTextFile(std::string_view fileName);
    }

}