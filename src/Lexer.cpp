#include "Lexer.h"
#include "Token.h"
#include "Utility.h"

namespace interpreter
{
    Lexer::Lexer(std::string_view input) : 
        mInput(input),
        mPosition(mInput.begin()),
        mReadPosition(mPosition + 1)
    {
        assert(!input.empty());

        if (mPosition != mInput.end())
        {
            mChar = *mPosition;
        }
    }

    Token Lexer::NextToken()
    {
        Token token;

        SkipWhiteSpace();

        switch (mChar)
        {
        case 0: utility::AssignToToken(token, TokenType::ENDF, "\0");
            return token;
            break;
        case '=': utility::AssignToToken(token, TokenType::ASSIGN, &mChar);
            break;
        case ';': utility::AssignToToken(token, TokenType::SEMICOLON, &mChar);
            break;
        case '(': utility::AssignToToken(token, TokenType::LPAREN, &mChar);
            break;
        case ')': utility::AssignToToken(token, TokenType::RPAREN, &mChar);
            break;
        case ',': utility::AssignToToken(token, TokenType::COMMA, &mChar);
            break;
        case '+': utility::AssignToToken(token, TokenType::PLUS, &mChar);
            break;
        case '{': utility::AssignToToken(token, TokenType::LBRACE, &mChar);
            break;
        case '}': utility::AssignToToken(token, TokenType::RBRACE, &mChar);
            break;
        default:
            if (utility::IsLetter(mChar))
            {
                std::string_view identifier{ ReadIdentifier() };
                utility::AssignToToken(token, utility::DeriveIdentifierToken(identifier), identifier);
                return token;
            }
            else if (utility::IsDigit(mChar))
            {
                std::string_view number{ ReadNumber() };
                utility::AssignToToken(token, TokenType::INT, number);
                return token;
            }
            else 
            {
                utility::AssignToToken(token, TokenType::ILLEGAL, &mChar);
            }
        }

        AdvanceCharacter();
        return token;
    }

    std::vector<Token> Lexer::Tokenize()
    {
        std::vector<Token> tokens;
        Token token{ NextToken() };
        while (token.mType != TokenType::ENDF)
        {
            tokens.push_back(token);
            token = NextToken();
        }

        tokens.push_back(token);    // add EOF token
        return tokens;
    }

    void Lexer::AdvanceCharacter()
    {
        if (mReadPosition == mInput.end())
        {
            mChar = 0;
            return;
        }

        mPosition = mReadPosition;
        ++mReadPosition;

        mChar = *mPosition;
    }

    void Lexer::SkipWhiteSpace()
    {
        while (mChar == ' ' || mChar == '\t' || mChar == '\r')
        {
            AdvanceCharacter();
        }
    }

    std::string_view Lexer::ReadIdentifier()
    {
        for (; mReadPosition != mInput.end() && utility::IsLetter(*mReadPosition); mReadPosition++)
        {
        }

        std::string_view result{ mPosition, mReadPosition };
        mChar = *mReadPosition;
        return result;
    }

    std::string_view Lexer::ReadNumber()
    {
        for (; mReadPosition != mInput.end() && utility::IsDigit(*mReadPosition); mReadPosition++)
        {
        }

        std::string_view result{ mPosition, mReadPosition };
        mChar = *mReadPosition;
        return result;
    }
}