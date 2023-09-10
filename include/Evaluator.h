#pragma once
#ifdef INTERPRETER_DLL_EXPORT
#define INTERPRETER_DLL __declspec(dllexport)
#else
#define INTERPRETER_DLL __declspec(dllimport)
#endif

#include "ForwardDeclares.h"

namespace interpreter
{
    struct DebugObject
    {
        static std::vector<interpreter::ObjectWeakPtr>& GetDebugObjects();
    };

    enum class TokenType;
    namespace ast
    {
        class Node;
    }

    namespace Evaluator
    {
        ObjectSharedPtr Evaluate(ast::Node* node, const EnvironmentSharedPtr& env);

        ObjectSharedPtr EvaluateProgram(const ast::Program& program, const EnvironmentSharedPtr& env);

        ObjectSharedPtr EvaluateIfExpression(const ast::IfExpression& ifExpression, const EnvironmentSharedPtr& env);    // deals with statements so it requires an environment
        ObjectSharedPtr EvaluateConditionBlockStatement(const ast::ConditionBlockStatement& statement, const EnvironmentSharedPtr& env);
        ObjectSharedPtr EvaluateBlockStatement(const ast::BlockStatement& statement, const EnvironmentSharedPtr& env);

        std::vector<ObjectSharedPtr> EvaluateExpressions(const std::vector<ExpressionUniquePtr>& expressions, const EnvironmentSharedPtr& env);
        ObjectSharedPtr ApplyFunction(const ObjectSharedPtr& function, std::vector<ObjectSharedPtr>&& arguments);
        EnvironmentSharedPtr ExtendFunctionEnvironment(const FunctionType& function, std::vector<ObjectSharedPtr>&& arguments);

        ObjectSharedPtr EvaluateIdentifier(const ast::PrimitiveExpression& primitive, const EnvironmentSharedPtr& env);
        ObjectSharedPtr EvaluatePrefixExpression(TokenType operatorToken, const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluatePrefixBangOperatorExpression(const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluatePrefixMinusOperatorExpression(const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluateInfixExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluateInfixIntegerExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right);

        bool IsTruthy(const ObjectSharedPtr& obj);
        bool IsError(const ObjectSharedPtr& obj);

        ObjectSharedPtr UnwrapReturnValue(const ObjectSharedPtr& object);

        // Static objects
        ObjectSharedPtr GetNativeBoolObject(bool value);
        ObjectSharedPtr GetNativeNullObject();
    };
}