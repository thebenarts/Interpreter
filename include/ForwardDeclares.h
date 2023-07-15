#pragma once
#include <stdint.h>
#include <memory>
#include <variant>

namespace interpreter
{
    class Lexer;
    class Parser;
    class Token;

    namespace ast
    {
        class Expression;
        class Statement;
        class LetStatement;
        class ReturnStatement;
        class ExpressionStatement;
        class BlockStatement;
        class ConditionBlockStatement;
        class Expression;
        class PrimitiveExpression;
        class InfixExpression;
        class IfExpression;
        class Program;
    }

    typedef int16_t CharacterRange;
    typedef int64_t Number;
    typedef std::variant<std::string, Number, bool> TokenPrimitive;

    typedef std::unique_ptr<ast::Expression> ExpressionUniquePtr;
    typedef std::unique_ptr<ast::Statement> StatementUniquePtr;
    typedef std::unique_ptr<ast::LetStatement> LetStatementUniquePtr;
    typedef std::unique_ptr<ast::ReturnStatement> ReturnStatementUniquePtr;
    typedef std::unique_ptr<ast::ExpressionStatement> ExpressionStatementUniquePtr;
    typedef std::unique_ptr<ast::BlockStatement> BlockStatementUniquePtr;
    typedef std::unique_ptr<ast::ConditionBlockStatement> ConditionBlockStatementUniquePtr;
    typedef std::unique_ptr<ast::Expression> ExpressionUniquePtr;
    typedef std::unique_ptr<ast::PrimitiveExpression> PrimitiveExpressionUniquePtr;
    typedef std::unique_ptr<ast::InfixExpression> InfixExpressionUniquePtr;
    typedef std::unique_ptr<ast::IfExpression> IfExpressionUniquePtr;
    typedef std::unique_ptr<ast::Program> ProgramUniquePtr;

    typedef std::unique_ptr<Lexer> LexerUniquePtr;
    typedef std::unique_ptr<Parser> ParserUniquePtr;

    typedef std::shared_ptr<Token> TokenSharedPtr;
}