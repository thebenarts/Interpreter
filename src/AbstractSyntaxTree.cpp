#include "AbstractSyntaxTree.h"

namespace interpreter {

    namespace ast
    {
        std::string Statement::Log()
        {
            return "";
        }

        std::string LetStatement::Log()
        {
            std::string result{ std::get<std::string>(mToken.mLiteral) };
            result.push_back(' ');
            result.append(std::get<std::string>(mIdentifier.mLiteral));
            result.append(" = ");

            if (mExpression)
            {
                result.append(mExpression->Log());
            }

            return result;
        }

        std::string ReturnStatement::Log()
        {
            std::string result{ std::get<std::string>(mToken.mLiteral) };
            result.push_back(' ');

            if (mExpression)
            {
                result.append(mExpression->Log());
            }

            return result;
        }

        std::string Expression::Log()
        {
            return "";
        }
    }
}
