#pragma once
#ifdef INTERPRETER_DLL_EXPORT
#define INTERPRETER_DLL __declspec(dllexport)
#else
#define INTERPRETER_DLL __declspec(dllimport)
#endif

#include "ForwardDeclares.h"

namespace interpreter
{
    enum class TokenType;
    namespace ast
    {
        class Node;
    }

    namespace Evaluator
    {
        ObjectSharedPtr Evaluate(ast::Node* node);
        ObjectSharedPtr EvaluateProgram(const ast::Program& program);

        ObjectSharedPtr EvaluatePrefixExpression(TokenType operatorToken, const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluatePrefixBangOperatorExpression(const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluatePrefixMinusOperatorExpression(const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluateInfixExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluateInfixIntegerExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right);
        ObjectSharedPtr EvaluateIfExpression(const ast::IfExpression& ifExpression);
        ObjectSharedPtr EvaluateConditionBlockStatement(const ast::ConditionBlockStatement& statement);
        ObjectSharedPtr EvaluateBlockStatement(const ast::BlockStatement& statement);

        bool IsTruthy(const ObjectSharedPtr& obj);
        bool IsError(const ObjectSharedPtr& obj);

        // Static objects
        ObjectSharedPtr GetNativeBoolObject(bool value);
        ObjectSharedPtr GetNativeNullObject();
    };
}