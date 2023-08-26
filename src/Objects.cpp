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
}
