#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <iostream>

namespace interpreter 
{
    class Token;

    class Lexer
    {
    public:
        Lexer(std::string_view);
        Token NextToken();
        std::vector<Token> Tokenize();

    private:
        void AdvanceCharacter();
        void SkipWhiteSpace();
        std::string_view ReadIdentifier();
        std::string_view ReadNumber();

        std::string mInput;
        std::string::iterator mPosition;
        std::string::iterator mReadPosition;
        char mChar;
    };
}

