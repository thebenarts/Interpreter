#include "Objects.h"
#include "Utility.h"
#include <sstream>

namespace interpreter
{

    // ------------------------------------------------------------ Integer Type -----------------------------------------------------

    ObjectType IntegerType::Type() const
    {
        return "int";
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
        return "bool";
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
        return "NULL";
    };

    std::string NullType::Inspect() const
    {
        return "nullptr";
    };

}


