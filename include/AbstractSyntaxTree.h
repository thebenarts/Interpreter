#pragma once
#include "Token.h"
#include "Utility.h"

namespace interpreter {

    namespace ast
    {
        // Operator precedence
        enum Precedence : uint8_t
        {
            LOWEST = 0,
            EQUALS = 1,                 // ==
            LESSGREATER = 1 << 1,       // > or <
            SUM = 1 << 2,               // +
            PRODUCT = 1 << 3,           // *
            PREFIX = 1 << 4,            // -X or !X
            CALL = 1 << 5,              // myFunction(X)
        };

        enum class NodeType : uint8_t
        {
            Node,
            Statement,
            Expression,
            LetStatement,
            ReturnStatement,
            ExpressionStatement,
            Program,
        };

        enum class ExpressionType
        {
            None,
            IdentifierExpression,
            IntegerExpression,
            PrefixExpression,
            InfixExpression
        };

        struct Node
        {
            Node():mNodeType(NodeType::Node){}

            NodeType mNodeType;
            Token mToken;
            virtual ~Node() {};
        };

        struct Statement : public Node
        {
            Statement() { mNodeType = NodeType::Statement; }
            virtual std::string Log();
            std::unique_ptr<Expression> mExpression;
            virtual ~Statement() {};
        };

        struct Expression : public Node
        {
            Expression() { mNodeType = NodeType::Expression; mExpressionType = ExpressionType::None; }
            ExpressionType mExpressionType;
            std::string Log();
            virtual ~Expression() {};
        };

        struct PrefixExpression : public Expression 
        {
            PrefixExpression() { mNodeType = NodeType::Expression; mExpressionType = ExpressionType::PrefixExpression; }
            // mToken inherited through Expression->Node->mToken will hold the prefix
            std::unique_ptr<Expression> mRightSideValue;
            virtual ~PrefixExpression() {};
        };

        struct LetStatement : public Statement
        {
            LetStatement() { mNodeType = NodeType::LetStatement; }
            virtual std::string Log() override;
            Token mIdentifier;

            virtual ~LetStatement() {};
        };

        struct ReturnStatement : public Statement 
        {
            ReturnStatement() { mNodeType = NodeType::ReturnStatement; }
            virtual std::string Log() override;
            virtual ~ReturnStatement() {};
        };

        struct ExpressionStatement : public Statement
        {
            ExpressionStatement() { mNodeType = NodeType::ExpressionStatement; }
            virtual ~ExpressionStatement() {};
        };

        struct Program
        {
            std::vector<std::unique_ptr<Statement>> mStatements;
        };
    }
}