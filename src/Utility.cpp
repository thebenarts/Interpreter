#include "Utility.h"
#include <iostream>

namespace interpreter
{
    namespace utility
    {
        bool IsLetter(char character)
        {
            return ('a' <= character && character <= 'z') || ('A' <= character && character <= 'Z') || character == '_';
        }

        bool IsDigit(char character)
        {
            return ('0' <= character && character <= '9');
        }

        // TODO: This assumes the string isn't holding a number larger than 2^63-1 or smaller than -2^63
        int64_t ToNumber(std::string_view literal)
        {
            int64_t number{};
            for (const char digit : literal)
            {
                VERIFY(IsDigit(digit))
                {
                    number *= 10;
                    number += 0x0F & digit;
                }
            }

            return number;
        }

        bool CompareTokens(const Token& left, const Token& right)
        {
            VERIFY(left.mType == right.mType) {}
            else
            {
                std::cout << "ERROR: Token Type mismatch: " << ConvertTokenTypeToString(left.mType) << " : "
                    << ConvertTokenTypeToString(right.mType) << std::endl;

                return false;
            }

            bool leftIsString{ std::holds_alternative<std::string>(left.mLiteral) };
            bool rightIsString{ std::holds_alternative<std::string>(right.mLiteral) };

            VERIFY(leftIsString == rightIsString) {}
            else
            {
                std::cout << "ERROR: Token Literals hold different types of values." << std::endl;

                return false;
            }

            if (leftIsString)
            {
                const auto leftValue{ std::get<std::string>(left.mLiteral) };
                const auto rightValue{ std::get<std::string>(right.mLiteral) };
                VERIFY(leftValue == rightValue) {}
            else
            {
                std::cout << "ERROR: Token values are not the same: " << leftValue <<
                    " : " << rightValue << std::endl;

                return false;
            }
            }
            else
            {
                const auto leftValue{ std::get<Number>(left.mLiteral) };
                const auto rightValue{ std::get<Number>(right.mLiteral) };
                VERIFY(leftValue == rightValue) {}
                else
                {
                    std::cout << "ERROR: Token values are not the same: " << leftValue <<
                        " : " << rightValue << std::endl;

                    return false;
                }
            }

            return true;
        }

        std::string ConvertTokenTypeToString(TokenType tokenType)
        {
            if (const auto tokenIter{ sTokenTypeToStringMap.find(tokenType) }; tokenIter != sTokenTypeToStringMap.end())
            {
                return tokenIter->second;
            }

            assert(false);
            return "NO TOKEN TYPE IN MAP";
        }

        void AssignToToken(Token& token, TokenType tokenType, std::string_view literal, CharacterRange* characterRange)
        {
            token.mType = tokenType;
            token.mLiteral.emplace<std::string>(literal);
            memcpy(token.mCharacterRange, characterRange, 2 * sizeof(CharacterRange));
        }

        void AssignToToken(Token& token, TokenType tokenType, const char literal, CharacterRange* characterRange)
        {
            token.mType = tokenType;
            std::string str{ literal };
            token.mLiteral.emplace<std::string>(str);
            memcpy(token.mCharacterRange, characterRange, 2 * sizeof(CharacterRange));
        }

        void AssignToToken(Token& token, TokenType tokenType, Number literal, CharacterRange* characterRange)
        {
            token.mType = tokenType;
            token.mLiteral.emplace<Number>(literal);
            memcpy(token.mCharacterRange, characterRange, 2 * sizeof(CharacterRange));
        }

        void AssignToToken(Token& token, TokenType tokenType, bool literal, CharacterRange* characterRange)
        {
            token.mType = tokenType;
            token.mLiteral.emplace<bool>(literal);
            memcpy(token.mCharacterRange, characterRange, 2 * sizeof(CharacterRange));
        }

        TokenType DeriveIdentifierToken(std::string_view literal)
        {
            std::string word{ literal };
            if (const auto keywordIter{ sKeywordsMap.find(word) }; keywordIter != sKeywordsMap.end())
            {
                return keywordIter->second;
            }

            return TokenType::IDENT;
        }

        std::string ReadTextFile(std::string_view fileName)
        {
            assert(!fileName.empty());

            // eat the file so we can determine the size
            std::ifstream input(fileName.data(), std::ios::in | std::ios::binary | std::ios::ate);
            VERIFY(input.good())
            {
                std::ifstream::pos_type fileSize{ input.tellg() };
                input.seekg(0, std::ios::beg);

                std::string bytes(static_cast<size_t>(fileSize), '\0');  // Need to use Parentheses to legally cast to size_t
                input.read(&bytes[0], fileSize);    // Note: this limits us to ASCI.

                return bytes;   // NRVO should make this a move operation
            }

            return {};
        }
    }
}
