#pragma once
#include "Token.h"
#include "Utility.h"

namespace interpreter {

    namespace ast
    {
        struct Node
        {
            Token mToken;
            virtual ~Node() {};
        };

        struct Statement : public Node
        {
            virtual ~Statement() {};
        };

        struct Expression : public Node
        {
            virtual ~Expression() {};
        };

        struct LetStatement : public Statement
        {
            Token mIdentifier;
            std::unique_ptr<Expression> mValue;

            virtual ~LetStatement() {};
        };

        struct ReturnStatement : public Statement 
        {
            std::unique_ptr<Expression> mValue;

            virtual ~ReturnStatement() {};
        };

        struct Program
        {
            std::vector<std::unique_ptr<Statement>> mStatements;
        };
    }
}