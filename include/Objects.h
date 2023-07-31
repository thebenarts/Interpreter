#pragma once
#include "ForwardDeclares.h"
#include "Utility.h"
#include "Logger.h"
#include <string>
#include <format>

namespace interpreter
{
    struct Object
    {
        virtual ObjectType Type() const = 0;
        virtual std::string Inspect() const = 0;
    };

    struct IntegerType : public Object
    {
        IntegerType() = default;
        IntegerType(UnsignedNumber num) {
            if (num > INT64_MAX)
            {
                LOG_MESSAGE(MessageType::ERRORS, std::format("Integer can't be represented in negative form due to inssuficent space: {}", num));
                assert(false);
            }
            mValue = utility::narrow_cast<Number>(num);
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