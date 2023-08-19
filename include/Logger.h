#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <source_location>
#include "AbstractSyntaxTree.h"

namespace interpreter
{
#define LOG(type,...) LogImplementation(type, std::source_location::current(), __VA_ARGS__)

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

    template<typename... T>
    void LogImplementation(MessageType type , const std::source_location location , const T&... args)
    {
        std::ostringstream stream;
        stream << location.file_name() << ' ' << location.line() << ' ' << location.function_name() << ' ';
        (stream << ... << args);

        LOG_MESSAGE(type, stream.str());
    }
}
