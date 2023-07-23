#pragma once
#include <string>
#include <vector>
#include "AbstractSyntaxTree.h"

namespace interpreter
{
    enum class MessageType
    {
        MESSAGE,
        WARNING,
        ERRORS,
    };

    struct Message
    {
        MessageType type;
        std::string message;
    };

    struct Logger
    {
        enum class LoggerType;

        Logger() = default;
        Logger(LoggerType type);
        ~Logger();

        static Logger& CommandLineLogger();
        static Logger& FileLogger();

        static void Log(MessageType type, const std::string& message);

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
    void LOG_MESSAGE(MessageType type, const std::string& message);

}
