#include "Lexer.h"
#include "Token.h"
#include "Utility.h"
#include "Logger.h"
#include <format>

namespace interpreter
{
    Lexer::Lexer(std::string_view input) :
        mInput(input),
        mPosition(mInput.begin()),
        mReadPosition(mPosition + 1),
        mLineNumber(0),
        mCharacterNumber(0)
    {
        assert(!input.empty());

        memset(mCharacterRange, 0, 2 * sizeof(CharacterRange));
        if (mPosition != mInput.end())
        {
            mChar = *mPosition;
        }

        Tokenize();
    }

    Token Lexer::AdvanceToken()
    {
        Token token;

        SkipWhiteSpace();

        token.mLineNumber = mLineNumber;
        switch (mChar)
        {
        case 0: utility::AssignToToken(token, TokenType::ENDF, "\0", SetCharacterRange());
            return token;
            break;
        case '=':
            if (PeekCharacter() == '=')
            {
                const std::string literal{ mChar,PeekCharacter() };
                utility::AssignToToken(token, TokenType::EQ, literal, SetCharacterRange(1));
                AdvanceCharacter();
            }
            else
            {
                utility::AssignToToken(token, TokenType::ASSIGN, mChar, SetCharacterRange());
            }
            break;
        case '+': utility::AssignToToken(token, TokenType::PLUS, mChar, SetCharacterRange());
            break;
        case '-': utility::AssignToToken(token, TokenType::MINUS, mChar, SetCharacterRange());
            break;
        case '!':
            if (PeekCharacter() == '=')
            {
                const std::string literal{ mChar,PeekCharacter() };
                utility::AssignToToken(token, TokenType::NOT_EQ, literal, SetCharacterRange(1));
                AdvanceCharacter();
            }
            else
            {
                utility::AssignToToken(token, TokenType::BANG, mChar, SetCharacterRange());
            }
            break;
        case '/': utility::AssignToToken(token, TokenType::SLASH, mChar, SetCharacterRange());
            break;
        case '*': utility::AssignToToken(token, TokenType::ASTERISK, mChar, SetCharacterRange());
            break;
        case '<': utility::AssignToToken(token, TokenType::LT, mChar, SetCharacterRange());
            break;
        case '>': utility::AssignToToken(token, TokenType::GT, mChar, SetCharacterRange());
            break;
        case ';': utility::AssignToToken(token, TokenType::SEMICOLON, mChar, SetCharacterRange());
            break;
        case '(': utility::AssignToToken(token, TokenType::LPAREN, mChar, SetCharacterRange());
            break;
        case ')': utility::AssignToToken(token, TokenType::RPAREN, mChar, SetCharacterRange());
            break;
        case ',': utility::AssignToToken(token, TokenType::COMMA, mChar, SetCharacterRange());
            break;
        case '{': utility::AssignToToken(token, TokenType::LBRACE, mChar, SetCharacterRange());
            break;
        case '}': utility::AssignToToken(token, TokenType::RBRACE, mChar, SetCharacterRange());
            break;
        default:
            if (utility::IsLetter(mChar))
            {
                std::string_view identifier{ ReadIdentifier() };
                // TODO: think of a way to unify range input.
                // Due to how we built character ranges we have to directly pass the range in the case of traversed ranges
                TokenType tokenType{ utility::DeriveIdentifierToken(identifier) };
                if (sBooleanTokens.contains(tokenType))
                {
                    utility::AssignToToken(token, tokenType, bool{ tokenType == TokenType::TRUE }, mCharacterRange);
                }
                else if (tokenType == TokenType::ELSE) // Brute force check for else if
                {
                    if (std::string_view elseIfString{ CheckElseIf() }; !elseIfString.empty())
                    {
                        utility::AssignToToken(token, TokenType::ELSE_IF, elseIfString, mCharacterRange);
                    }
                    else    // It's a regular else
                    {
                        utility::AssignToToken(token, TokenType::ELSE, identifier, mCharacterRange);
                    }
                }
                else
                {
                    utility::AssignToToken(token, tokenType, identifier, mCharacterRange);
                }
                return token;
            }
            else if (utility::IsDigit(mChar))
            {
                std::string_view number{ ReadNumber() };
                if (!utility::ValidateStringNumber(number))
                {
                    LOG_MESSAGE(MessageType::ERRORS, std::format("line: {} character rage: [ {} - {} ], Number can't fit into signed 64 bit integer: {}",
                        token.mLineNumber, mCharacterRange[0], mCharacterRange[1], number));

                    // For now we just send it as a 0.
                    utility::AssignToToken(token, TokenType::INT, Number(0) , mCharacterRange);
                    return token;
                }
                utility::AssignToToken(token, TokenType::INT, utility::ToNumber(number), mCharacterRange);
                return token;
            }
            else
            {
                utility::AssignToToken(token, TokenType::ILLEGAL, mChar, SetCharacterRange());
            }
        }

        AdvanceCharacter();
        return token;
    }

    void Lexer::Tokenize()
    {
        Token token{ AdvanceToken() };
        while (token.mType != TokenType::ENDF)
        {
            mTokens.push_back(token);
            token = AdvanceToken();
        }

        mTokens.push_back(token);    // add EOF token
    }

    CharacterRange* Lexer::SetCharacterRange(CharacterRange range /* = 0*/)
    {
        mCharacterRange[0] = mCharacterNumber;
        mCharacterRange[1] = mCharacterNumber + range;
        return mCharacterRange;
    }

    const std::vector<Token>& Lexer::GetTokens()
    {
        return mTokens;
    }

    void Lexer::AdvanceCharacter(bool hadNewLine /* = false*/)
    {
        if (mReadPosition == mInput.end())
        {
            mChar = 0;
            return;
        }

        mPosition = mReadPosition;
        ++mReadPosition;
        if (!hadNewLine)
        {
            ++mCharacterNumber;
        }

        mChar = *mPosition;
    }

    const char Lexer::PeekCharacter()
    {
        if (mReadPosition == mInput.end())
        {
            return 0;
        }

        return *mReadPosition;
    }

    void Lexer::SkipWhiteSpace()
    {
        const auto expression = [this]()->bool {return mChar == ' ' || mChar == '\t' || mChar == '\r' || mChar == '\n'; };
        bool newLine{ false };
        while (expression())
        {
            // TODO: think about a way to optimize this double check away.
            if (mChar == '\n')
            {
                ++mLineNumber;
                mCharacterNumber = 0;
                newLine = true;
            }
            AdvanceCharacter(newLine);
        }
    }

    std::string_view Lexer::ReadIdentifier()
    {
        mCharacterRange[0] = mCharacterNumber;
        for (; mReadPosition != mInput.end() && (utility::IsLetter(*mReadPosition) || utility::IsDigit(*mReadPosition)); mReadPosition++)
        {
        }

        mCharacterRange[1] = mCharacterRange[0] + (mReadPosition - mPosition) - 1;
        mCharacterNumber = mCharacterRange[1];
        std::string_view result{ mPosition, mReadPosition };
        AdvanceCharacter(); // Advances mReadPosition + 1 and mPosition gets advanced to where mReadPosition used to be
        return result;
    }


    std::string_view Lexer::ReadNumber()
    {
        mCharacterRange[0] = mCharacterNumber;
        for (; mReadPosition != mInput.end() && utility::IsDigit(*mReadPosition); mReadPosition++)
        {
        }

        mCharacterRange[1] = mCharacterRange[0] + (mReadPosition - mPosition) - 1;
        mCharacterNumber = mCharacterRange[1];
        std::string_view result{ mPosition, mReadPosition };
        AdvanceCharacter();
        return result;
    }

    std::string_view Lexer::CheckElseIf()
    {
        // We only enter this function when "else" has been encountered 
        // mPosition is on the last character of the identifier
        // mReadPosition should be one past the last character of the identifier. i.e. whitespace or "{" or statement
        // TODO: come back and optimize this.
        // Store the state of the lexer here in case it's just a regular else
        std::string::const_iterator elseStartIter{ mPosition - 4 };     // pointing to first e of "else"
        std::string::const_iterator elseEndIter{ mPosition };           // pointing to last e of "else"
        std::string::const_iterator readIter{ mReadPosition };          // pointing one past last e of "else"

        CharacterRange originalCharacterNumber{ mCharacterNumber };
        CharacterRange originalRange[2];
        memcpy(originalRange, mCharacterRange, 2 * sizeof(CharacterRange));
        int32_t originalLineNumber{ mLineNumber };
        char originalChar{ mChar };

        const auto RestoreOriginalLexerState = [&]()
        {
            mPosition = elseEndIter;
            mReadPosition = readIter;

            mCharacterNumber = originalCharacterNumber;
            memcpy(mCharacterRange, originalRange, 2 * sizeof(CharacterRange));
            mLineNumber = originalLineNumber;
            mChar = originalChar;
        };

        SkipWhiteSpace();
        // Check to see if next character is letter if so read it as an identifier and check if it's a keyword "if" 
        if (utility::IsLetter(mChar))
        {
            std::string_view identifier{ ReadIdentifier() };
            TokenType tokenType{ utility::DeriveIdentifierToken(identifier) };

            if (tokenType == TokenType::IF)
            {
                // We have found an "else if"
                mCharacterRange[0] = originalRange[0];
                return { elseStartIter,mPosition };
            }
        }

        // Wasn't "else if"
        RestoreOriginalLexerState();
        return {};
    }
}