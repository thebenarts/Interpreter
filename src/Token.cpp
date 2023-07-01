#include "Token.h"
#include <sstream>

namespace interpreter
{
    std::ostringstream& operator<<(std::ostringstream& out, const Token& token)
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
            Number boolean{ std::get<Number>(token.mLiteral) };
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
}