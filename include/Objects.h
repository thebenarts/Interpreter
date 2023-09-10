#pragma once
#include "ForwardDeclares.h"
#include "Utility.h"
#include "Logger.h"
#include <string>
#include <format>

namespace interpreter
{

#define NEW_ERROR(...) (NewError(MessageType::ERRORS, std::source_location::current(), __VA_ARGS__))

    namespace ObjectTypes
    {
        constexpr const char* INTEGER_OBJECT{ "int" };
        constexpr const char* BOOLEAN_OBJECT{ "bool" };
        constexpr const char* NULL_OBJECT{ "NULL" };
        constexpr const char* RETURN_OBJECT{ "RETURN_VALUE" };
        constexpr const char* ERROR_OBJECT{ "ERROR_VALUE" };
        constexpr const char* FUNCTION_OBJECT{ "FUNCTION" };
    }

    struct Object
    {
        virtual ObjectType Type() const = 0;
        virtual std::string Inspect() const = 0;
    };

    struct IntegerType : public Object
    {
        IntegerType() = default;
        IntegerType(Number num) {
            if (num > INT64_MAX)
            {
                LOG_MESSAGE(MessageType::ERRORS, std::format("Integer can't be represented in negative form due to inssuficent space: {}", num));
                assert(false);
            }
            mValue = num;
        }

        virtual ObjectType Type() const override;
        virtual std::string Inspect() const override;

        Number mValue;
    };

    struct BoolType : public Object
    {
        BoolType() = default;
        BoolType(bool boolean) : mValue(boolean) {}

        virtual ObjectType Type() const override;
        virtual std::string Inspect() const override;
        // Note overriding the bool operator is to make working with "NullType" easier not for actual "BoolType"

        bool mValue;
    };

    struct NullType : public Object
    {
        virtual ObjectType Type() const override;
        virtual std::string Inspect() const override;
    };

    struct ReturnType : public Object
    {
        explicit ReturnType(ObjectSharedPtr obj) : mValue(obj) {}

        virtual ObjectType Type() const override;
        virtual std::string Inspect() const override;

        ObjectSharedPtr mValue;
    };

    struct ErrorType : public Object
    {
        ErrorType() = default;
        ErrorType(std::string_view context, std::string_view message) : mContext(context), mMessage(message) {}

        virtual ObjectType Type() const override;
        virtual std::string Inspect() const override;

        std::string mContext;
        std::string mMessage;
    };

    struct FunctionType : public Object
    {
        FunctionType(std::vector<ExpressionUniquePtr>&& parameters, BlockStatementUniquePtr&& body, std::weak_ptr<Environment>environment) :
            mParameters(std::move(parameters)), mBody(std::move(body)), mEnvironment(environment) {}

        virtual ObjectType Type() const override;
        virtual std::string Inspect() const override;

        std::vector<ExpressionUniquePtr> mParameters;
        BlockStatementUniquePtr mBody;
        std::shared_ptr<Environment> mEnvironment;  // TODO: might need to refactor this. Prototyping to make higher order functions to work for now.
    };

    struct Environment : public std::unordered_map<std::string, ObjectSharedPtr>
    {
        static EnvironmentSharedPtr NewEnvironment();
        static EnvironmentSharedPtr NewEnclosedEnvironment(EnvironmentSharedPtr outer);

        ObjectSharedPtr Get(std::string_view key) const;
        ObjectSharedPtr Set(std::string_view key, const ObjectSharedPtr&& obj);

        EnvironmentSharedPtr mOuter;
    };

    template<typename... T>
    std::shared_ptr<ErrorType> NewError(MessageType type, std::source_location location, const T&... args)
    {
        std::ostringstream stream;
        FormatLocation_impl(stream, location);
        const auto locationSize{ stream.view().size() };
        FormatVariadicArguments_impl(stream, args...);
        const auto fullMessageView{ stream.view() };
        LOG_MESSAGE(type, fullMessageView);
        return std::make_shared<ErrorType>(std::string_view{ fullMessageView.begin(), fullMessageView.begin() + locationSize }, std::string_view{ fullMessageView.begin() + locationSize, fullMessageView.end() });
    };
}