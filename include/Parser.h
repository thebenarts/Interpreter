#pragma once
#include "Token.h"
#include "Utility.h"
#include "Lexer.h"
#include "AbstractSyntaxTree.h"

namespace interpreter
{
    typedef std::unique_ptr<ast::Expression> ExpressionUniquePtr;
    typedef std::unique_ptr<ast::Statement> StatementUniquePtr;
    typedef std::unique_ptr<ast::LetStatement> LetStatementUniquePtr;
    typedef std::unique_ptr<ast::Program> ProgramUniquePtr;


    typedef std::unique_ptr<Lexer> LexerUniquePtr;
    typedef std::vector<Token>::const_iterator ConstTokenIterator;

    class Parser    // Friend of Lexer
    {
    public:
        Parser(std::unique_ptr<Lexer> lexer);

        ProgramUniquePtr ParseProgram();

    private:
        StatementUniquePtr ParseStatement();
        LetStatementUniquePtr ParseLetStatement();

        // Lexer utilities
        void AdvanceToken();
        const Token* GetCurrentTokenAndAdvance();
        const Token* GetCurrentToken();
        const Token* GetNextToken();
        std::tuple<const Token*, const Token*> GetTokens();
        bool PeekTokenIs(TokenType tokenType);
        bool CurrentTokenIs(TokenType tokenType);
        bool TokenIs(const Token& token, TokenType tokenType);
        // Lexer variables
        LexerUniquePtr mLexer;
        ConstTokenIterator mCurrent;
        ConstTokenIterator mNext;
        ConstTokenIterator mEnd;
    };
}