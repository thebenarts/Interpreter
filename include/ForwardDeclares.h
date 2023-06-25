#pragma once
#include <stdint.h>
#include <memory>

namespace interpreter
{
    class Lexer;
    class Parser;

    namespace ast
    {
        class Expression;
        class Statement;
        class LetStatement;
        class ReturnStatement;
        class ExpressionStatement;
        class Expression;
        class Program;
    }

    typedef int16_t CharacterRange;

    typedef std::unique_ptr<ast::Expression> ExpressionUniquePtr;
    typedef std::unique_ptr<ast::Statement> StatementUniquePtr;
    typedef std::unique_ptr<ast::LetStatement> LetStatementUniquePtr;
    typedef std::unique_ptr<ast::ReturnStatement> ReturnStatementUniquePtr;
    typedef std::unique_ptr<ast::ExpressionStatement> ExpressionStatementUniquePtr;
    typedef std::unique_ptr<ast::Expression> ExpressionUniquePtr;
    typedef std::unique_ptr<ast::Program> ProgramUniquePtr;

    typedef std::unique_ptr<Lexer> LexerUniquePtr;
    typedef std::unique_ptr<Parser> ParserUniquePtr;
}