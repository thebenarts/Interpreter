#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <iostream>

namespace interpreter 
{
    class Token;
    class Parser;

    class Lexer
    {
    public:
        friend Parser;
        Lexer(std::string_view);

        const std::vector<Token>& GetTokens();
        std::vector<Token> GetTokenCopies() { return mTokens; };

    private:
        Token AdvanceToken();
        void AdvanceCharacter();
        const char PeekCharacter();
        void SkipWhiteSpace();
        std::string_view ReadIdentifier();
        std::string_view ReadNumber();

        void Tokenize();

        std::string mInput;
        std::string::iterator mPosition;
        std::string::iterator mReadPosition;
        char mChar;

        std::vector<Token> mTokens;
    };
}

