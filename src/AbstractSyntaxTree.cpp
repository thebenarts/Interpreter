#include "AbstractSyntaxTree.h"
#include <sstream>

namespace interpreter {

    namespace ast
    {
        // ------------------------------------------------------------ Let Statement -----------------------------------------------------

        std::optional<Token> LetStatement::TokenNode() { return mToken; }
        std::optional<Token> LetStatement::StatementNode() { return {}; }

        std::string LetStatement::Log()
        {
            std::ostringstream result;

            if (auto tokenNode{ TokenNode() }; tokenNode)
            {
                result << *tokenNode << " ";
            }

            if (mIdentifier)
            {
                if (auto identifierToken{ mIdentifier->TokenNode() })
                {
                    result << *identifierToken;
                }
            }

            if (mValue)
            {
                result << mValue->Log();
            }

            result << ";";
            return result.str();
        }

        // ------------------------------------------------------------ Return Statement -----------------------------------------------------

        std::optional<Token> ReturnStatement::TokenNode() { return mToken; }
        std::optional<Token> ReturnStatement::StatementNode() { return {}; }

        std::string ReturnStatement::Log()
        {
            std::ostringstream result;

            if (auto tokenNode{ TokenNode() }; tokenNode)
            {
                result << *tokenNode << " ";
            }

            if (mValue)
            {
                result << mValue->Log();
            }

            result << ";";
            return result.str();
        }

        // ------------------------------------------------------------ Expression Statement -----------------------------------------------------

        std::optional<Token> ExpressionStatement::TokenNode() { return mToken; }
        std::optional<Token> ExpressionStatement::StatementNode() { return {}; }

        std::string ExpressionStatement::Log()
        {
            std::ostringstream result;

            if (mValue)
            {
                result << mValue->Log();
            }
            return result.str();
        }

        // ------------------------------------------------------------ Primitive Expression -----------------------------------------------------

        std::optional<Token> PrimitiveExpression::TokenNode() { return mToken; }
        std::optional<Token> PrimitiveExpression::ExpressionNode() { return {}; }

        std::string PrimitiveExpression::Log()
        {
            std::ostringstream result;

            if (auto tokenNode{ TokenNode() })
            {
                result << *tokenNode;
            }

            return result.str();
        }

        // ------------------------------------------------------------ Prefix Expression -----------------------------------------------------

        std::optional<Token> PrefixExpression::TokenNode() { return mToken; }
        std::optional<Token> PrefixExpression::ExpressionNode() { return {}; }

        std::string PrefixExpression::Log()
        {
            std::ostringstream result;

            const auto token{ TokenNode() };
            VERIFY(token);
            result << "(" << *token;
            if (mRightSideValue)
            {
                result << mRightSideValue->Log();
            }
            result << ")";

            return result.str();
        }

        // ------------------------------------------------------------ Infix Expression -----------------------------------------------------

        std::optional<Token> InfixExpression::TokenNode() { return mToken; }
        std::optional<Token> InfixExpression::ExpressionNode() { return {}; }

        std::string InfixExpression::Log()
        {
            std::ostringstream result;

            result << "(";
            VERIFY(mLeftExpression);
            result << mLeftExpression->Log();
            result << " " <<  mToken << " ";

            if (mRightExpression)
            {
                result << mRightExpression->Log();
            }

            result << ")";
            return result.str();
        }

        // ------------------------------------------------------------ Program -----------------------------------------------------

        std::string Program::Log()
        {
            std::ostringstream result;

            for (auto& statement : mStatements)
            {
                if (statement)
                {
                    result << statement->Log() << std::endl;
                }
            }

            return result.str();
        }

    } // namespace ast

}   // namespace interpreter
