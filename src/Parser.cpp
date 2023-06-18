#include "Parser.h"

namespace interpreter
{
    Parser::Parser(std::unique_ptr<Lexer> lexer) : mLexer(std::move(lexer))
    {
        VERIFY(!mLexer->mTokens.empty())
        {
            mCurrent = mLexer->mTokens.begin();
            mNext = mCurrent + 1;
            mEnd = mLexer->mTokens.end();
        }
    }
    void Parser::AdvanceToken()
    {
        if (mNext == mEnd)
        {
            return;
        }

        mCurrent = mNext;
        mNext++;
    }

    const Token* Parser::GetCurrentTokenAndAdvance()
    {
        const Token* token{ GetCurrentToken() };
        AdvanceToken();
        return token;
    }

    const Token* Parser::GetCurrentToken()
    {
        if (mCurrent != mEnd)
        {
            return &(*mCurrent);
        }

        return nullptr;
    }

    const Token* Parser::GetNextToken()
    {
        if (mNext != mEnd)
        {
            return &(*mNext);
        }

        return nullptr;
    }

    std::tuple<const Token*, const Token*> Parser::GetTokens()
    {
        return { GetCurrentToken(),GetNextToken() };
    }

    ProgramUniquePtr Parser::ParseProgram()
    {
        auto program{ std::make_unique<ast::Program>()};
        while (GetCurrentToken() && !CurrentTokenIs(TokenType::ENDF))
        {
            StatementUniquePtr statement{ ParseStatement() };
            if (statement)
            {
                program->mStatements.push_back(std::move(statement));
            }
            AdvanceToken();
        }

        return program;
    }

    StatementUniquePtr Parser::ParseStatement()
    {
        const Token* token{ GetCurrentToken() };
        VERIFY(token)
        {
            switch (token->mType)
            {
            case TokenType::LET :
                return ParseLetStatement();
                break;
            default:
                return nullptr;
                break;
            }
        }
        return nullptr;
    }

    LetStatementUniquePtr Parser::ParseLetStatement()
    {
        auto statement{ std::make_unique<ast::LetStatement>() };
        // We only land here after checking the CurrentToken in ParseStatement so it is safe to dereference without checking here 
        statement->mToken = *GetCurrentToken();

        if (!PeekTokenIs(TokenType::IDENT))
        {
            return nullptr;
        }

        // PeekTokenIs already checks for null so yet again this is safe to dereference
        statement->mIdentifier = *GetNextToken();
        AdvanceToken();

        if (!PeekTokenIs(TokenType::ASSIGN))
        {
            return nullptr;
        }

        // TODO: Add logic. for now we skip until we find a semicolon
        while (GetCurrentToken() && !CurrentTokenIs(TokenType::SEMICOLON))
        {
            AdvanceToken();
        }

        return statement;
    }

    bool Parser::PeekTokenIs(TokenType tokenType)
    {
        if (const Token * peekToken{ GetNextToken() })
        {
            return TokenIs(*peekToken, tokenType);
        }

        return false;
    }

    bool Parser::CurrentTokenIs(TokenType tokenType)
    {
        if (const Token* currentToken{ GetCurrentToken() })
        {
            return TokenIs(*currentToken, tokenType);
        }

        return false;
    }

    bool Parser::TokenIs(const Token& token, TokenType tokenType)
    {
        return token.mType == tokenType;
    }
}