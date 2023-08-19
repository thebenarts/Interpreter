#include "Token.h"
#include "Utility.h"
#include <sstream>
#include <assert.h>

namespace interpreter
{
    std::ostream& operator<<(std::ostream& out, const Token& token)
    {
        [[likely]] if (sStringTokens.contains(token.mType))
        {
            out << std::get<std::string>(token.mLiteral);
        }
        else if (sNumberTokens.contains(token.mType))
        {
            out << std::get<Number>(token.mLiteral);
        }
        else if (sBooleanTokens.contains(token.mType))
        {
            bool boolean{ std::get<bool>(token.mLiteral) };
            if (boolean)
            {
                out << "true";
            }
            else
            {
                out << "false";
            }
        }

        return out;
    }

    std::ostream& operator<<(std::ostream& out, const TokenType& tokenType)
    {
        out << utility::ConvertTokenTypeToString(tokenType);
        return out;
    }

    std::string Token::ToString(TokenPrimitive primitive)
    {
        std::ostringstream out;
        std::visit([&out](const auto& arg)
            {
                using Type = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<std::monostate, Type>)
                {
                    out << "NULL";
                }
                else if constexpr (std::is_same_v<std::string, Type>)
                {
                    out << arg;
                }
                else if constexpr (std::is_same_v<Number, Type>)
                {
                    out << arg;
                }
                else if constexpr (std::is_same_v<bool, Type>)
                {
                    if (arg)
                    {
                        out << "true";
                    }
                    else
                    {
                        out << "false";
                    }
                }
                else
                {
                    assert(false);
                }
            }, primitive);

        return out.str();
    }
}