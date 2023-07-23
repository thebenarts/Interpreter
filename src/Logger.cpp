#include "Logger.h"
#include <memory>
#include <time.h>
#include <format>
#include <chrono>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace interpreter
{
    Logger::Logger(LoggerType type) : mLoggerType(type)
    {

    }

    Logger::~Logger()
    {
        auto getTime = []() -> std::string {
            std::ostringstream ostream;
            std::time_t now = std::time({});
            ostream << std::put_time(std::localtime(&now), "%Y-%m-%d_%H-%M-%S");
            return ostream.str();
        };

        if (mLoggerType == LoggerType::FILE)
        {
            if (CreateDirectoryA("../output", nullptr) || ERROR_ALREADY_EXISTS == GetLastError())
            {
                std::ofstream out{ std::format("../output/Interpreter-{}.txt", getTime())};
                if (!out)
                {
                    std::cout << "Error: creating file\n";
                }

                for (auto& msg : mData)
                {
                    out << msg.message << '\n';
                }
            }
        }
    }

    Logger& Logger::CommandLineLogger()
    {
        static std::shared_ptr<Logger> sCmdLogger{ std::make_shared<Logger>(LoggerType::CMD) };
        return *sCmdLogger;
    }

    Logger& Logger::FileLogger()
    {
        static std::shared_ptr<Logger> sFileLogger{ std::make_shared<Logger>(LoggerType::FILE) };
        return *sFileLogger;
    }

    void Logger::Log(MessageType type, const std::string& message)
    {
#ifdef DEBUG_ON
        std::cout << message << '\n';
#endif
        FileLogger().mData.emplace_back(type, message);
    }

    void LOG_MESSAGE(ast::Node* node, MessageType type /*= MessageType::MESSAGE*/)
    {
        Logger::Log(type, node->Log());
    }

    void LOG_MESSAGE(std::unique_ptr<ast::Node>& node, MessageType type /*= MessageType::MESSAGE*/)
    {
        Logger::Log(type, node->Log());
    }

    void LOG_MESSAGE(MessageType type, const std::string& message)
    {
        Logger::Log(type, message);
    }
}