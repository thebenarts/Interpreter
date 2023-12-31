#pragma once
#include "ForwardDeclares.h"
#include "Utility.h"
#include "Logger.h"
#include <string>
#include <format>

namespace interpreter
{
    namespace ObjectTypes
    {
        constexpr const char* INTEGER_OBJECT{ "int" };
        constexpr const char* BOOLEAN_OBJECT{ "bool" };
        constexpr const char* NULL_OBJECT{ "NULL" };
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

        bool mValue;
    };

    struct NullType : public Object
    {
        virtual ObjectType Type() const override;
        virtual std::string Inspect() const override;
    };
}