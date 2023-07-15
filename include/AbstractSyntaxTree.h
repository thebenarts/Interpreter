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

        const std::unordered_map<TokenType, Precedence> sOperatorPrecedenceMap
        {
            {TokenType::EQ,EQUALS},
            {TokenType::NOT_EQ, EQUALS},
            {TokenType::LT, LESSGREATER},
            {TokenType::GT, LESSGREATER},
            {TokenType::PLUS, SUM},
            {TokenType::MINUS, SUM},
            {TokenType::SLASH, PRODUCT},
            {TokenType::ASTERISK, PRODUCT}
        };

        enum class NodeType : uint8_t
        {
            Node,
            Statement,
            Expression,
            LetStatement,
            ReturnStatement,
            ExpressionStatement,
            BlockStatement,
            ConditionBlockStatement,
            Program,
        };

        enum class ExpressionType
        {
            None,
            PrimitiveExpression,
            IdentifierExpression,
            IntegerExpression,
            BooleanExpression,
            PrefixExpression,
            InfixExpression,
            IfExpression
        };

        struct Node
        {
            virtual std::optional<Token> TokenNode() = 0;
            virtual std::string Log() = 0;
            NodeType mNodeType;
            virtual ~Node() {};
        };

        struct Statement : public Node
        {
            virtual std::optional<Token> StatementNode() = 0;
            virtual ~Statement() {};
        };

        struct Expression : public Node
        {
            virtual std::optional<Token> ExpressionNode() = 0;
            ExpressionType mExpressionType;
            virtual ~Expression() {};
        };

        struct LetStatement : public Statement
        {
            LetStatement() { mNodeType = NodeType::LetStatement; }
            virtual ~LetStatement() {};

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> StatementNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken;                       // let Token
            ExpressionUniquePtr mIdentifier;
            ExpressionUniquePtr mValue;         // Expression to assign 
        };

        struct ReturnStatement : public Statement 
        {
            ReturnStatement() { mNodeType = NodeType::ReturnStatement; }
            virtual ~ReturnStatement() {};

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> StatementNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken;                       // return Token
            ExpressionUniquePtr mValue;         // Return value expression
        };

        struct BlockStatement : public Statement
        {
            BlockStatement() { mNodeType = NodeType::BlockStatement; }
            virtual ~BlockStatement() {};

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> StatementNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken; // the { token
            std::vector<StatementUniquePtr> mStatements;
        };

        struct ConditionBlockStatement : public Statement
        {
            ConditionBlockStatement() { mNodeType = NodeType::ConditionBlockStatement; }
            virtual ~ConditionBlockStatement() {};

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> StatementNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken; // the conditional token == "if" || "else if" || "else"
            ExpressionUniquePtr mCondition;     // This will be empty if it's an "else"
            BlockStatementUniquePtr mBlock;     // holds the statement 
        };

        struct ExpressionStatement : public Statement
        {
            ExpressionStatement() { mNodeType = NodeType::ExpressionStatement; }
            virtual ~ExpressionStatement() {};

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> StatementNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken;                       // The first token of the expression
            ExpressionUniquePtr mValue;         // Rest of the expression
        };

        struct PrimitiveExpression : public Expression
        {
            PrimitiveExpression() { mNodeType = NodeType::Expression; mExpressionType = ExpressionType::PrimitiveExpression; }
            virtual ~PrimitiveExpression(){}

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> ExpressionNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken;
        };

        struct PrefixExpression : public Expression 
        {
            PrefixExpression() { mNodeType = NodeType::Expression; mExpressionType = ExpressionType::PrefixExpression; }
            virtual ~PrefixExpression() {};

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> ExpressionNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken;
            Token mOperator;
            ExpressionUniquePtr mRightSideValue;
        };

        struct InfixExpression : public Expression
        {
            InfixExpression() { mNodeType = NodeType::Expression; mExpressionType = ExpressionType::InfixExpression; }
            virtual ~InfixExpression() {};

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> ExpressionNode() override;
            virtual std::string Log() override;

            ExpressionUniquePtr mLeftExpression;
            Token mToken;   // The operator token, e.g. +,- etc.
            ExpressionUniquePtr mRightExpression;
        };

        struct IfExpression : public Expression
        {
            IfExpression() { mNodeType = NodeType::Expression; mExpressionType = ExpressionType::IfExpression; }
            virtual ~IfExpression(){}

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> ExpressionNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken;   // if token
            ConditionBlockStatementUniquePtr mIfConditionBlock;
            std::vector<ConditionBlockStatementUniquePtr> mElseIfBlocks;
            BlockStatementUniquePtr mAlternative;
        };

        struct FunctionExpression : public Expression
        {
            FunctionExpression() { mNodeType = NodeType::Expression; mExpressionType = ExpressionType::IfExpression; }
            virtual ~FunctionExpression(){}

            virtual std::optional<Token> TokenNode() override;
            virtual std::optional<Token> ExpressionNode() override;
            virtual std::string Log() override;

            // Variables
            Token mToken;   // fn token
            std::vector<ExpressionUniquePtr> mParameters;
            BlockStatementUniquePtr mBody;
        };

        struct Program
        {
            std::vector<StatementUniquePtr> mStatements;

            std::string Log();
        };
    }
}