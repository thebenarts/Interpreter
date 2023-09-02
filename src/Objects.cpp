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

    // ------------------------------------------------------------ Function Type -----------------------------------------------------

    ObjectType FunctionType::Type() const
    {
        return ObjectTypes::FUNCTION_OBJECT;
    };

    std::string FunctionType::Inspect() const
    {
        std::ostringstream stream;
        stream << "fn (";
        for (int i = 0; i != mParameters.size(); i++)
        {
            if (const auto & parameter{ mParameters[i] })
            {
                stream << parameter->Log();
                if (i != mParameters.size() - 1)
                {
                    stream << ", ";
                }
            }
        }
        stream << ")\n{\n\t" << mBody->Log() << "\n}";

        return stream.str();
    };

    // ------------------------------------------------------------ Environment -----------------------------------------------------

    EnvironmentSharedPtr Environment::NewEnvironment()
    {
        return std::make_shared<Environment>();
    }

    EnvironmentSharedPtr Environment::NewEnclosedEnvironment(EnvironmentSharedPtr outer)
    {
        const auto env{ NewEnvironment() };
        env->mOuter = outer;
        return env;
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

    ObjectSharedPtr Environment::Set(std::string_view key, const ObjectSharedPtr&& obj)
    {
        const auto pair{ insert_or_assign(key.data(),std::move(obj)) };
        return pair.first->second;
    }
}
