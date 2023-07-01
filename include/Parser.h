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

    private:
        void RegisterParseFunctionPointers();

        // Parse Statements
        StatementUniquePtr ParseStatement();
        LetStatementUniquePtr ParseLetStatement();
        ReturnStatementUniquePtr ParseReturnStatement();
        ExpressionStatementUniquePtr ParseExpressionStatement();
        // Parse Expressions
        ExpressionUniquePtr ParseExpression(ast::Precedence precedence);
        ExpressionUniquePtr ParsePrimitiveExpression();
        ExpressionUniquePtr ParsePrefixExpression();

        // Lexer utilities
        void AdvanceToken();
        const Token* GetCurrentTokenAndAdvance();
        const Token* GetCurrentToken();
        const Token* GetNextToken();
        std::tuple<const Token*, const Token*> GetTokens();
        bool PeekTokenIs(TokenType tokenType);
        bool CurrentTokenIs(TokenType tokenType);
        bool TokenIs(const Token& token, TokenType tokenType);
        bool ExpectPeekTokenIs(TokenType tokenType);
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