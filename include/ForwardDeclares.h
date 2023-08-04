#pragma once
#include <stdint.h>
#include <memory>
#include <variant>
#include <string>

namespace interpreter
{
    class Lexer;
    class Parser;
    class Token;
    class Message;
    class Object;

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
    typedef uint64_t UnsignedNumber;
    typedef int64_t Number;
    typedef std::variant<std::monostate, std::string, Number, bool> TokenPrimitive;
    typedef std::string ObjectType;

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
    typedef std::unique_ptr<Object> ObjectUniquePtr;
    typedef std::shared_ptr<Object> ObjectSharedPtr;

    typedef std::unique_ptr<Lexer> LexerUniquePtr;
    typedef std::unique_ptr<Parser> ParserUniquePtr;

    typedef std::shared_ptr<Token> TokenSharedPtr;
    typedef std::shared_ptr<Message> MessageSharedPtr;
}