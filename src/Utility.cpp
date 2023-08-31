#include "Utility.h"
#include "Parser.h"
#include "Objects.h"
#include <iostream>
#include <sstream>

namespace interpreter
{
    namespace utility
    {
        ProgramUniquePtr CreateProgramFromText(std::string_view text)
        {
            interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(text) };
            interpreter::Parser parser{ std::move(lexer) };
            return parser.ParseProgram();
        }

        ProgramUniquePtr CreateProgramFromFile(std::string_view path)
        {
            std::string parserInput{ interpreter::utility::ReadTextFile(path) };
            interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
            interpreter::Parser parser{ std::move(lexer) };
            return  parser.ParseProgram();
        }

        std::string GetAndValidateIdentifierIsFree(ast::Expression& expression, const EnvironmentSharedPtr& env)
        {
            assert(expression.mExpressionType == ast::ExpressionType::IdentifierExpression);
            if (const auto token{ expression.TokenNode() })
            {
                assert(token->mType == TokenType::IDENT);
                std::string_view identifier{ std::get<std::string>(token->mLiteral) };
                if (const auto object{ env->Get(identifier) })
                {
                    LOG(ERRORS, "Identifier: ", identifier, " already exists in environment with value: ", object->Inspect());
                }
                return std::string{ identifier };
            }

            return {};
        }

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

        bool ValidateStringNumber(std::string_view literal)
        {
            // 9223 3720 3685 4775 807       ----------- 19 digits ------------
            // -9223 3720 3685 4775 808     
            // If we really care about that 1 extra for negative numbers we can store numbers as unsigned in the Tokens
            static constexpr const char* INT64_T_MAX{ "9223372036854775807" };

            // It's most likely that the string is less than 19 digits long. If so there is no need to test further.
            if (literal.size() < 19)
            {
                return true;
            }

            // If it's exactly 19 digits long we have to check.
            if (literal.size() == 19)
            {
                for (int i = 0; i != 19; i++)
                {
                    if (literal[i] < INT64_T_MAX[i])
                    {
                        return true;
                    }
                    else if (literal[i] > INT64_T_MAX[i])
                    {
                        return false;
                    }
                }

                return true;    // Means it's equal.
            }

            return false;
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

        std::string DeriveType(TokenPrimitive primitive)
        {
            std::ostringstream out;
            std::visit([&out](auto& arg)
                {
                    out << TypeName<std::decay_t<decltype(arg)>>();
                }, primitive);

            return out.str();
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
