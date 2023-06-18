#pragma once
#include "Token.h"
#include "Lexer.h"
#include "AbstractSyntaxTree.h"

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
        // Test utilities
        bool CompareTokens(const std::span<Token> expected, const std::span<Token> result);
        bool TestLetStatement(ast::Statement* letStatement, std::string_view identifier);


        bool IsLetter(char character);
        bool IsDigit(char character);

        std::string ConvertTokenTypeToString(TokenType tokenType);
        void AssignToToken(Token& token, TokenType tokenType, std::string_view literal);
        void AssignToToken(Token& token, TokenType tokenType, const char literal);
        TokenType DeriveIdentifierToken(std::string_view literal);
        std::string ReadTextFile(std::string_view fileName);
    }


}