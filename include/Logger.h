#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <source_location>
#include "AbstractSyntaxTree.h"

namespace interpreter
{
#define LOG(type,...) do {Log_impl(type, std::source_location::current(), __VA_ARGS__);}    \
while (0)

#define LOG_INTERNAL(type,location, ...) do {Log_impl(type,location, __VA_ARGS__);}         \
while (0)

    enum MessageType : uint8_t
    {
        UNDEFINED,
        MESSAGE,
        WARNING,
        ERRORS,
    };

    struct Message
    {
        Message() = default;
        Message(MessageType type, std::string_view message) : mType(type), mMessage(message) {}

        MessageType mType;
        std::string mMessage;
    };

    struct Logger
    {
        enum class LoggerType;

        Logger() = default;
        Logger(LoggerType type);
        ~Logger();

        static Logger& CommandLineLogger();
        static Logger& FileLogger();
        static const MessageType& SetLoggerSeverity(MessageType severity);
        static const MessageType& GetLoggerSeverity();

        static void Log(MessageType type, std::string_view message);

    private:
        static std::string MessageString(MessageType type);
        enum class LoggerType
        {
            CMD,
            FILE,
        };
        LoggerType mLoggerType;
        std::vector<Message> mData;
    };

    void LOG_MESSAGE(ast::Node* node, MessageType type = MessageType::MESSAGE);
    void LOG_MESSAGE(std::unique_ptr<ast::Node>& node, MessageType type = MessageType::MESSAGE);
    void LOG_MESSAGE(MessageType type, std::string_view message);
    void LOG_MESSAGE(std::string_view message);


    inline void FormatLocation_impl(std::ostringstream& stream, const std::source_location location)
    {
        stream << location.file_name() << ' ' << location.line() << ' ' << location.function_name() << ' ';
    }

    inline std::string FormatLocation(const std::source_location location)
    {
        std::ostringstream stream;
        FormatLocation_impl(stream, location);

        return stream.str();
    }

    template<typename... T>
    void FormatVariadicArguments_impl(std::ostringstream& stream, const T&... args)
    {
        (stream << ... << args);
    }

    template<typename... T>
    std::string FormatVariadicArguments(const T&... args)
    {
        std::ostringstream stream;
        FormatVariadicArgumets_impl(args...);
        return stream.str();
    }

    template<typename... T>
    std::string FormatString(const std::source_location location, const T&... args)
    {
        std::ostringstream stream;
        FormatLocation_impl(stream, location);
        FormatVariadicArguments_impl(stream, args...);

        return stream.str();
    }

    template<typename... T>
    void Log_impl(MessageType type , const std::source_location location , const T&... args)
    {
        LOG_MESSAGE(type, FormatString(location, args...));
    }
}
