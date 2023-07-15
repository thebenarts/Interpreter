#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <iostream>

#include "ForwardDeclares.h"

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

        void AdvanceCharacter(bool hadNewLine = false);
        const char PeekCharacter();
        void SkipWhiteSpace();
        std::string_view ReadIdentifier();
        std::string_view ReadNumber();

        std::string_view CheckElseIf();

        void Tokenize();
        CharacterRange* SetCharacterRange(CharacterRange range = 0);

        std::string mInput;
        std::string::const_iterator mPosition;
        std::string::const_iterator mReadPosition;
        char mChar;
        int32_t mLineNumber;
        CharacterRange mCharacterNumber;
        CharacterRange mCharacterRange[2];

        std::vector<Token> mTokens;
    };
}

