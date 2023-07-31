#pragma once

#include "Token.h"
#include "Lexer.h"
#include "ForwardDeclares.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <optional>

#include <fstream>
#include <assert.h>
#include <memory>

#define VERIFY(expression) assert(expression); if(expression) 

#define VERIFY_MESSAGE(expression,message) if (!expression) std::cout << message << std::endl;  \
    assert(expression); \
    if(expression)

#define IF_LOG(expression, message) if(!expression) std::cout << message << std::endl; \
    if (expression)

namespace interpreter
{
    namespace utility
    {
        bool IsLetter(char character);
        bool IsDigit(char character);
        int64_t ToNumber(std::string_view literal);

        bool CompareTokens(const Token& left, const Token& right);
        std::string ConvertTokenTypeToString(TokenType tokenType);

        void AssignToToken(Token& token, TokenType tokenType, std::string_view literal, CharacterRange* characterRange);
        void AssignToToken(Token& token, TokenType tokenType, const char literal, CharacterRange* characterRange);
        void AssignToToken(Token& token, TokenType tokenType, Number literal, CharacterRange* characterRange);
        void AssignToToken(Token& token, TokenType tokenType, bool literal, CharacterRange* characterRange);
        TokenType DeriveIdentifierToken(std::string_view literal);
        std::string ReadTextFile(std::string_view fileName);

        std::string DeriveType(TokenPrimitive primitive);

        template <typename T>
        std::string TypeName()
        {
            typedef typename std::remove_reference<T>::type TR;
            std::unique_ptr<char, void(*)(void*)> own
            (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr);
#else
                nullptr,
#endif
                std::free
            );

            std::string result = own != nullptr ? own.get() : typeid(TR).name();
            if (std::is_const<TR>::value)
            {
                result += " const";
            }
            if (std::is_volatile<TR>::value)
            {
                result += " volatile";
            }
            if (std::is_lvalue_reference<T>::value)
            {
                result += "&";
            }
            else if (std::is_rvalue_reference<T>::value)
            {
                result += "&&";
            }

            return result;
        };

        template<typename Derived, typename Base, typename Del>
        std::unique_ptr<Derived, Del> StaticUniquePtrCast(std::unique_ptr<Base, Del>&& pointer)
        {
            auto derived{ static_cast<Derived*>(pointer.release()) };
            return std::unique_ptr<Derived, Del>(derived, std::move(pointer.get_deleter()));
        };

        template<typename Derived, typename Base, typename Del>
        std::unique_ptr<Derived, Del> AssertUniquePtrCast(std::unique_ptr<Base, Del>&& pointer)
        {
            auto derived{ dynamic_cast<Derived*>(pointer.release()) };
            VERIFY(!derived)
            {
                std::cout << "Dynamic cast failed:" << TypeName<Base>() << "to " << TypeName<Derived>();
                return nullptr;
            }

            return std::unique_ptr<Derived, Del>(derived, std::move(pointer.get_deleter()));
        };

        template<typename T, typename U>
        constexpr T narrow_cast(U&& u) noexcept
        {
            return static_cast<T>(std::forward<U>(u));
        }
    }
}