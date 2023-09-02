#pragma once
#include <stdint.h>
#include <memory>
#include <variant>
#include <string>
#include <vector>

namespace interpreter
{
    class Lexer;
    class Parser;
    struct Token;
    struct Message;

    struct Object;
    struct IntegerType;
    struct BoolType;
    struct NullType;
    struct ReturnType;
    struct ErrorType;
    struct FunctionType;
    struct Environment;

    namespace ast
    {
        struct Expression;
        struct Statement;
        struct LetStatement;
        struct ReturnStatement;
        struct ExpressionStatement;
        struct BlockStatement;
        struct ConditionBlockStatement;
        struct Expression;
        struct PrimitiveExpression;
        struct InfixExpression;
        struct IfExpression;
        struct Program;
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

    typedef std::shared_ptr<Environment> EnvironmentSharedPtr;

    typedef std::unique_ptr<Lexer> LexerUniquePtr;
    typedef std::unique_ptr<Parser> ParserUniquePtr;

    typedef std::shared_ptr<Token> TokenSharedPtr;
    typedef std::shared_ptr<Message> MessageSharedPtr;
}