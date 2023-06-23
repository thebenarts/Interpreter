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
            token.mLiteral = literal;
            memcpy(token.mCharacterRange, characterRange, 2 * sizeof(CharacterRange));
        }

        void AssignToToken(Token& token, TokenType tokenType, const char literal, CharacterRange* characterRange)
        {
            token.mType = tokenType;
            token.mLiteral = literal;
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
            
            std::ifstream::pos_type fileSize{ input.tellg() };
            input.seekg(0, std::ios::beg);

            std::string bytes(static_cast<size_t>(fileSize),'\0');  // Need to use Parentheses to legally cast to size_t
            input.read(&bytes[0], fileSize);    // Note: this limits us to ASCI.

            return bytes;   // NRVO should make this a move operation
        }
    }
}
