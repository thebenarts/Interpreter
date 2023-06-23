#pragma once
#include "Utility.h"
#include "AbstractSyntaxTree.h"

namespace interpreter
{
    typedef std::vector<Token>::const_iterator ConstTokenIterator;

    class Parser    // Friend of Lexer
    {
    public:
        Parser(LexerUniquePtr lexer);

        ProgramUniquePtr ParseProgram();

    private:
        StatementUniquePtr ParseStatement();
        LetStatementUniquePtr ParseLetStatement();
        ReturnStatementUniquePtr ParseReturnStatement();

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
        // Lexer variables
        LexerUniquePtr mLexer;
        ConstTokenIterator mCurrent;
        ConstTokenIterator mNext;
        ConstTokenIterator mEnd;
    };
}