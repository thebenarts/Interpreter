#include "Objects.h"
#include "Utility.h"
#include <sstream>

namespace interpreter
{

    // ------------------------------------------------------------ Integer Type -----------------------------------------------------

    ObjectType IntegerType::Type() const
    {
        return ObjectTypes::INTEGER_OBJECT;
    };

    std::string IntegerType::Inspect() const
    {
        std::ostringstream out;
        out << mValue;
        return out.str();
    };

    // ------------------------------------------------------------ Bool Type -----------------------------------------------------

    ObjectType BoolType::Type() const
    {
        return ObjectTypes::BOOLEAN_OBJECT;
    };

    std::string BoolType::Inspect() const
    {
        if (mValue)
        {
            return "true";
        }

        return "false";
    };

    // ------------------------------------------------------------ Null Type -----------------------------------------------------

    ObjectType NullType::Type() const
    {
        return ObjectTypes::NULL_OBJECT;
    };

    std::string NullType::Inspect() const
    {
        return "nullptr";
    };

    // ------------------------------------------------------------ Return Type -----------------------------------------------------

    ObjectType ReturnType::Type() const
    {
        return ObjectTypes::RETURN_OBJECT;
    };

    std::string ReturnType::Inspect() const
    {
        return mValue ? mValue->Inspect() : "";
    };

    // ------------------------------------------------------------ Error Type -----------------------------------------------------

    ObjectType ErrorType::Type() const
    {
        return ObjectTypes::ERROR_OBJECT;
    };

    std::string ErrorType::Inspect() const
    {
        return mMessage;
    };

    // ------------------------------------------------------------ Error Type -----------------------------------------------------

    EnvironmentSharedPtr Environment::NewEnvironment()
    {
        return std::make_shared<Environment>();
    }

    ObjectSharedPtr Environment::Get(std::string_view key) const
    {
        if (const auto iterator{ find(key.data()) }; iterator != end())
        {
            return iterator->second;
        }
        else if (mOuter)
        {
            return mOuter->Get(key);
        }

        return nullptr;
    }

    ObjectSharedPtr Environment::Set(std::string_view key, ObjectSharedPtr&& obj)
    {
        const auto pair{ insert_or_assign(key.data(),std::move(obj)) };
        return pair.first->second;
    }
}
