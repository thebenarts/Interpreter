#pragma once
#include "Token.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>

#include <fstream>
#include <assert.h>

namespace interpreter
{
    namespace utility
    {
        bool CompareTokens(const std::span<Token> expected, const std::span<Token> result);

        bool IsLetter(char character);
        bool IsDigit(char character);

        std::string ConvertTokenTypeToString(TokenType tokenType);
        void AssignToToken(Token& token, TokenType tokenType, std::string_view literal);
        TokenType DeriveIdentifierToken(std::string_view literal);
        std::string ReadTextFile(std::string_view fileName);
    }


}