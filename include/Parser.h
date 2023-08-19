#pragma once
#include "Utility.h"
#include "AbstractSyntaxTree.h"
#include <functional>
#include <unordered_map>

namespace interpreter
{
    typedef std::vector<Token>::const_iterator ConstTokenIterator;
    typedef std::function<ExpressionUniquePtr()> PrefixParseFunctionPtr;
    typedef std::function<ExpressionUniquePtr(ExpressionUniquePtr)> InfixParseFunctionPtr;
    typedef std::unordered_map<TokenType, PrefixParseFunctionPtr> PrefixFunctionPtrMap;
    typedef std::unordered_map<TokenType, InfixParseFunctionPtr> InfixFunctionPtrMap;

    class Parser    // Friend of Lexer
    {
    public:
        Parser(LexerUniquePtr lexer);

        ProgramUniquePtr ParseProgram();

        // Evaluate
        static ObjectSharedPtr Evaluate(ast::Node* node);
    private:
        void RegisterParseFunctionPointers();

        // Parse Statements
        StatementUniquePtr ParseStatement();
        LetStatementUniquePtr ParseLetStatement();
        ReturnStatementUniquePtr ParseReturnStatement();
        ExpressionStatementUniquePtr ParseExpressionStatement();
        BlockStatementUniquePtr ParseBlockStatement();
        ConditionBlockStatementUniquePtr ParseConditionBlockStatement();
        // Parse Expressions
        ExpressionUniquePtr ParseExpression(ast::Precedence precedence);
        ExpressionUniquePtr ParsePrimitiveExpression();
        ExpressionUniquePtr ParsePrefixExpression();
        ExpressionUniquePtr ParseGroupedExpression();
        ExpressionUniquePtr ParseIfExpression();
        ExpressionUniquePtr ParseFunctionExpression();
        std::vector<ExpressionUniquePtr> ParseFunctionParameters();
        std::vector<ExpressionUniquePtr> ParseCallArguments();

        ExpressionUniquePtr ParseInfixExpression(ExpressionUniquePtr leftExpression);
        ExpressionUniquePtr ParseCallExpression(ExpressionUniquePtr leftExpression);

        // Expression precedence helpers
        ast::Precedence GetNextPrecedence();
        ast::Precedence GetCurrentPrecedence();
        ast::Precedence GetPrecedence(const Token& token);

        // Evaluate
        static ObjectSharedPtr GetNativeBoolObject(bool value);
        static ObjectSharedPtr GetNativeNullObject();
        static ObjectSharedPtr EvaluatePrefixExpression(TokenType operatorToken, const ObjectSharedPtr& right);
        static ObjectSharedPtr EvaluatePrefixBangOperatorExpression(const ObjectSharedPtr& right);
        static ObjectSharedPtr EvaluatePrefixMinusOperatorExpression(const ObjectSharedPtr& right);
        static ObjectSharedPtr EvaluateInfixExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right);
        static ObjectSharedPtr EvaluateInfixIntegerExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right);

        // Lexer utilities
        void AdvanceToken();
        const Token* GetCurrentTokenAndAdvance();
        const Token* GetCurrentToken();
        const Token* GetNextToken();
        std::tuple<const Token*, const Token*> GetTokens();
        bool NextTokenIs(TokenType tokenType);
        bool CurrentTokenIs(TokenType tokenType);
        bool TokenIs(const Token& token, TokenType tokenType);
        bool ExpectNextTokenIs(TokenType tokenType);
        // Parse variables
        PrefixFunctionPtrMap mPrefixFunctionPtrMap;
        InfixFunctionPtrMap mInfixFunctionPtrMap;
        
        // Lexer variables
        LexerUniquePtr mLexer;
        ConstTokenIterator mCurrent;
        ConstTokenIterator mNext;
        ConstTokenIterator mEnd;
    };
}