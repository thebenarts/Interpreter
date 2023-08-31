#include "Evaluator.h"
#include "Objects.h"
#include "Token.h"
#include "AbstractSyntaxTree.h"

namespace interpreter::Evaluator
{
    ObjectSharedPtr Evaluate(ast::Node* node, const EnvironmentSharedPtr& env)
    {
        VERIFY(node);   // To catch issues.

        // Helper for PrimitiveExpressions
        const auto GetAndValidateTokenPrimtive = [](ast::Expression* node) -> TokenPrimitive {
            const auto optionalToken{ node->TokenNode() };
            assert(optionalToken);
            return optionalToken->mLiteral;
        };

        switch (node->mNodeType)
        {
        case ast::NodeType::ExpressionStatement:
            // TODOBB: these could be changed to static casts as we hold metadata, but keeping them dynamic for a bit to make sure everything works
            if (const auto expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(node) })
            {
                return Evaluate(expressionStatement->mValue.get(), env);
            }
            assert(false);
            break;
        case ast::NodeType::LetStatement:
            if (const auto letStatement{ dynamic_cast<ast::LetStatement*>(node) })
            {
                const auto value{ Evaluate(letStatement->mValue.get(),env) };
                if (IsError(value))
                {
                    return value;
                }

                VERIFY(letStatement->mIdentifier)
                {
                    if (const auto identifier{ utility::GetAndValidateIdentifierIsFree(*letStatement->mIdentifier.get(),env) }; !identifier.empty())
                    {
                        env->Set(identifier, std::move(value));
                    }
                }
            }
            else
            {
                assert(false);
            }
            break;
        case ast::NodeType::ConditionBlockStatement:
            if (const auto conditionBlockStatement{ dynamic_cast<ast::ConditionBlockStatement*>(node) })
            {
                return EvaluateConditionBlockStatement(*conditionBlockStatement, env);
            }
            ASSERT_MESSAGE(false, MessageType::ERRORS, "Statement wasn't ConditionBlockStatement");
            break;
        case ast::NodeType::BlockStatement:
            if (const auto blockStatement{ dynamic_cast<ast::BlockStatement*>(node) })
            {
                return EvaluateBlockStatement(*blockStatement, env);
            }
            ASSERT_MESSAGE(false, MessageType::ERRORS, "Statement wasn't BlockStatement");
            break;
        case ast::NodeType::ReturnStatement:
            if (const auto returnStatement{ dynamic_cast<ast::ReturnStatement*>(node) })
            {
                if (const auto returnValue{ Evaluate(returnStatement->mValue.get(), env) })
                {
                    return IsError(returnValue) ? returnValue : std::make_shared<ReturnType>(returnValue);
                }
                ASSERT_AND_RETURN(false, nullptr);
            }
            ASSERT_MESSAGE(false, MessageType::ERRORS, "Statement wasn't ReturnStatement");
            break;
        case ast::NodeType::Program:
            if (const auto program{ dynamic_cast<ast::Program*>(node) })
            {
                return EvaluateProgram(*program, env);
            }
            ASSERT_MESSAGE(false, MessageType::ERRORS, "Statement wasn't Program");
            break;
        case ast::NodeType::Expression:
            if (const auto expression{ dynamic_cast<ast::Expression*>(node) })
            {
                if (expression->mExpressionType == ast::ExpressionType::IntegerExpression)
                {
                    const auto primitive{ GetAndValidateTokenPrimtive(expression) };
                    return std::make_shared<IntegerType>(std::get<Number>(primitive));
                }
                else if (expression->mExpressionType == ast::ExpressionType::IdentifierExpression)
                {
                    const auto primitiveExpression{ dynamic_cast<ast::PrimitiveExpression*>(expression) };
                    ASSERT_MESSAGE(primitiveExpression, MessageType::ERRORS, "Expression wasn't primitive expression");
                    return EvaluateIdentifier(*primitiveExpression, env);
                }
                else if (expression->mExpressionType == ast::ExpressionType::BooleanExpression)
                {
                    const auto primitive{ GetAndValidateTokenPrimtive(expression) };
                    return GetNativeBoolObject(std::get<bool>(primitive));
                }
                else if (expression->mExpressionType == ast::ExpressionType::PrefixExpression)
                {
                    const auto prefixExpression{ dynamic_cast<ast::PrefixExpression*>(expression) };
                    const auto right{ Evaluate(prefixExpression->mRightSideValue.get(), env) };
                    return IsError(right) ? right : EvaluatePrefixExpression(prefixExpression->mOperator.mType, right);
                }
                else if (expression->mExpressionType == ast::ExpressionType::InfixExpression)
                {
                    const auto infixExpression{ dynamic_cast<ast::InfixExpression*>(expression) };

                    const auto left{ Evaluate(infixExpression->mLeftExpression.get(), env) };
                    if (IsError(left))
                    {
                        return left;
                    }

                    const auto right{ Evaluate(infixExpression->mRightExpression.get(), env) };
                    if (IsError(right))
                    {
                        return right;
                    }

                    return EvaluateInfixExpression(infixExpression->mToken.mType, left, right);
                }
                else if (expression->mExpressionType == ast::ExpressionType::IfExpression)
                {
                    const auto ifExpression{ dynamic_cast<ast::IfExpression*>(expression) };
                    return EvaluateIfExpression(*ifExpression, env);
                }
            }
            break;
        default:
            ASSERT_MESSAGE(false, MessageType::ERRORS, "Couldn't figure out what to evaluate.");
            break;
        }

        return nullptr;
    }

    ObjectSharedPtr EvaluateProgram(const ast::Program& program, const EnvironmentSharedPtr& env)
    {
        // we want to hold onto result since even if it's not explicit we want to return the last result.
        ObjectSharedPtr result;
        for (const auto& statement : program.mStatements)
        {
            if (result = Evaluate(statement.get(), env))
            {
                if (result->Type() == ObjectTypes::RETURN_OBJECT)
                {
                    if (const auto returnResult{ dynamic_cast<ReturnType*>(result.get()) })
                    {
                        return returnResult->mValue;
                    }
                    return NEW_ERROR("Object type didn't match expected type : ", result->Type());
                }
                else if (result->Type() == ObjectTypes::ERROR_OBJECT)
                {
                    // Probably pointless check, but for now it's safer to check.
                    if (const auto error{ dynamic_cast<ErrorType*>(result.get()) }; !error)
                    {
                        return NEW_ERROR("Object type didn't match expected type : ", result->Type());
                    }

                    return result;
                }
            }
        }

        return result;
    }

    ObjectSharedPtr EvaluateIfExpression(const ast::IfExpression& ifExpression, const EnvironmentSharedPtr& env)
    {
        // Check the if statement
        if (const auto ifBlockResult{ Evaluate(ifExpression.mIfConditionBlock.get(), env) })
        {
            return ifBlockResult;
        }

        // Check the else if branches
        for (const auto& elseIfConditionBlock : ifExpression.mElseIfBlocks)
        {
            if (const auto optionalBlockResult{ Evaluate(elseIfConditionBlock.get(), env) })
            {
                return optionalBlockResult;
            }
        }

        // return the else branch
        if (ifExpression.mAlternative)
        {
            if (const auto elseBlockResult{ Evaluate(ifExpression.mAlternative.get(), env) })
            {
                return elseBlockResult;
            }
        }

        return GetNativeNullObject();
    }

    ObjectSharedPtr EvaluateConditionBlockStatement(const ast::ConditionBlockStatement& statement, const EnvironmentSharedPtr& env)
    {
        if (statement.mCondition)
        {
            if (const auto condition{ Evaluate(statement.mCondition.get(), env) })
            {
                if (IsError(condition))
                {
                    return condition;
                }

                if (IsTruthy(condition))
                {
                    const auto blockResult{ Evaluate(statement.mBlock.get(), env) };
                    return blockResult ? blockResult : GetNativeNullObject();
                }
            }
        }

        return GetNativeNullObject();
    }

    ObjectSharedPtr EvaluateBlockStatement(const ast::BlockStatement& statement, const EnvironmentSharedPtr& env)
    {
        for (const auto& statement : statement.mStatements)
        {
            if (const auto result{ Evaluate(statement.get(), env) }; result && (result->Type() == ObjectTypes::RETURN_OBJECT || result->Type() == ObjectTypes::ERROR_OBJECT))
            {
                return result;
            }
        }

        return GetNativeNullObject();
    }

    ObjectSharedPtr EvaluateIdentifier(const ast::PrimitiveExpression& primitive, const EnvironmentSharedPtr& env)
    {
        assert(primitive.mToken.mType == TokenType::IDENT);
        const auto& identifier{ std::get<std::string>(primitive.mToken.mLiteral) };
        if (const auto associatedObject{ env->Get(identifier) })
        {
            return associatedObject;
        }

        return NEW_ERROR("Identifier not found: ", identifier);
    }

    ObjectSharedPtr EvaluatePrefixExpression(TokenType operatorToken, const ObjectSharedPtr& right)
    {
        switch (operatorToken)
        {
        case TokenType::BANG:   // "!"
            return EvaluatePrefixBangOperatorExpression(right);

        case TokenType::MINUS:  // "-"
            return EvaluatePrefixMinusOperatorExpression(right);
        }
        return NEW_ERROR("Prefix operator not supported: (", operatorToken, ")");
    }

    ObjectSharedPtr EvaluatePrefixBangOperatorExpression(const ObjectSharedPtr& right)
    {
        // TODOBB: swap dynamic_cast to static_cast once it's confirmed to work.
        if (right->Type() == "bool")
        {
            const auto boolObject{ dynamic_cast<BoolType*>(right.get()) };
            if (boolObject->mValue)
            {
                return GetNativeBoolObject(false);
            }

            return GetNativeBoolObject(true);
        }
        else if (right->Type() == "int")
        {
            const auto intObject{ dynamic_cast<IntegerType*>(right.get()) };
            if (intObject->mValue == 0)
            {
                return GetNativeBoolObject(true);
            }

            return GetNativeBoolObject(false);
        }
        else if (right->Type() == "NULL")
        {
            return GetNativeBoolObject(true);
        }

        return GetNativeBoolObject(false);
    }

    ObjectSharedPtr EvaluatePrefixMinusOperatorExpression(const ObjectSharedPtr& right)
    {
        if (right->Type() != ObjectTypes::INTEGER_OBJECT)
        {
            return NEW_ERROR("No Prefix (-) Evaluator for: ", right->Type());
        }

        const auto intObject{ dynamic_cast<IntegerType*>(right.get()) };        // Just leaving the check in for now in case something goes awfully wrong.
        VERIFY(intObject)
        {
            intObject->mValue = ~intObject->mValue;
            intObject->mValue += 1;
            return right;
        }

        return GetNativeNullObject();
    }

    ObjectSharedPtr EvaluateInfixExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right)
    {
        VERIFY(left && right)
        {
            if (left->Type() != right->Type())
            {
                return NEW_ERROR("Type mismatch: ", left->Type(), " ", operatorToken, " ", right->Type());
            }

            if (left->Type() == ObjectTypes::INTEGER_OBJECT)
            {
                return EvaluateInfixIntegerExpression(operatorToken, left, right);
            }
            else if (left->Type() == ObjectTypes::BOOLEAN_OBJECT || left->Type() == ObjectTypes::NULL_OBJECT)
            {
                switch (operatorToken)
                {
                case TokenType::EQ:     // '=='
                    return GetNativeBoolObject(left.get() == right.get());
                case TokenType::NOT_EQ: // '!='
                    return GetNativeBoolObject(left.get() != right.get());
                default:
                    return NEW_ERROR(left->Type(), " doesn't support : ", operatorToken);
                }
            }
        }

        return GetNativeNullObject();
    }

    ObjectSharedPtr EvaluateInfixIntegerExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right)
    {
        const auto leftInt{ dynamic_cast<IntegerType*>(left.get()) };
        const auto rightInt{ dynamic_cast<IntegerType*>(right.get()) };
        VERIFY(leftInt && rightInt)
        {
            switch (operatorToken)
            {
            case TokenType::PLUS:   // '+'
                return std::make_shared<IntegerType>(leftInt->mValue + rightInt->mValue);
            case TokenType::MINUS:  // '-'
                return std::make_shared<IntegerType>(leftInt->mValue - rightInt->mValue);
            case TokenType::ASTERISK:  // '*'
                return std::make_shared<IntegerType>(leftInt->mValue * rightInt->mValue);
            case TokenType::SLASH:  // '/'
                return std::make_shared<IntegerType>(leftInt->mValue / rightInt->mValue);
            case TokenType::LT:     // '<'
                return GetNativeBoolObject(leftInt->mValue < rightInt->mValue);
            case TokenType::GT:     // '>'
                return GetNativeBoolObject(leftInt->mValue > rightInt->mValue);
            case TokenType::EQ:     // '=='
                return GetNativeBoolObject(leftInt->mValue == rightInt->mValue);
            case TokenType::NOT_EQ: // '!='
                return GetNativeBoolObject(leftInt->mValue != rightInt->mValue);
            default:
                return NEW_ERROR("Operator : ", operatorToken, " not supported by Number types.");
            }
        }

        return GetNativeNullObject();
    }

    bool IsTruthy(const ObjectSharedPtr& object)
    {
        if (!object)
        {
            return false;
        }
        else if (object->Type() == ObjectTypes::BOOLEAN_OBJECT)
        {
            // It might be better to just cast it and check the value.
            if (object.get() == GetNativeBoolObject(true).get())
            {
                return true;
            }
            else
            {
                false;
            }
        }
        else if (object->Type() == ObjectTypes::INTEGER_OBJECT)
        {
            const auto intObject{ dynamic_cast<IntegerType*>(object.get()) };
            ASSERT_MESSAGE_AND_RETURN(intObject, MessageType::ERRORS, false, "Object is not Integer type.");
            if (intObject->mValue <= 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        return false;
    }

    bool IsError(const ObjectSharedPtr& object)
    {
        if (object)
        {
            return object->Type() == ObjectTypes::ERROR_OBJECT;
        }

        return false;
    }

    ObjectSharedPtr GetNativeBoolObject(bool value)
    {
        static std::shared_ptr<BoolType> nativeTrue{ std::make_shared<BoolType>(true) };
        static std::shared_ptr<BoolType> nativeFalse{ std::make_shared<BoolType>(false) };

        if (value)
        {
            return nativeTrue;
        }

        return nativeFalse;
    }

    ObjectSharedPtr GetNativeNullObject()
    {
        static std::shared_ptr<NullType> nativeNull{ std::make_shared<NullType>() };

        return nativeNull;
    }
}