#pragma once

#include "Token.h"
#include "Lexer.h"
#include "ForwardDeclares.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>

#include <fstream>
#include <assert.h>

# define VERIFY(expression) assert(expression); if(expression) 

namespace interpreter
{
    namespace utility
    {
        bool IsLetter(char character);
        bool IsDigit(char character);

        std::string ConvertTokenTypeToString(TokenType tokenType);
        void AssignToToken(Token& token, TokenType tokenType, std::string_view literal, CharacterRange* characterRange);
        void AssignToToken(Token& token, TokenType tokenType, const char literal, CharacterRange* characterRange);
        TokenType DeriveIdentifierToken(std::string_view literal);
        std::string ReadTextFile(std::string_view fileName);
    }


}