#pragma once
#include <iostream>
#include <mutex>
#include <ctime>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <memory>

enum class LogLevel
{
    Info = 0,
    Debug,
    Warning,
    Error,
    Fatal
};

static std::string EventToString(LogLevel level) // 移除引用，因为不需要修改
{
    switch (level)
    {
    case LogLevel::Info:
        return "Info";
    case LogLevel::Debug:
        return "Debug";
    case LogLevel::Warning:
        return "Warning";
    case LogLevel::Error:
        return "Error";
    case LogLevel::Fatal:
        return "Fatal";
    default:
        return "Unknown";
    }
}

static std::string GetCurrTime()
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm now_tm;
    localtime_r(&now_time_t, &now_tm);


    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now_tm);

    return std::string(buffer);
}
class LogPolicy
{
public:
    virtual void synclog(const std::string &logmessage) = 0;
    virtual ~LogPolicy() = default; // 改为默认实现
};

class MonitorLogPolicy : public LogPolicy
{
public:
    void synclog(const std::string &logmessage) override
    {
        std::lock_guard<std::mutex> lock(_mtx);
        std::cout << logmessage << std::endl;
    }

private:
    std::mutex _mtx;
};

class Logger
{
public:
    Logger() : _strategy(std::make_unique<MonitorLogPolicy>())
    {
    }

    // 设置日志策略
    void setPolicy(std::unique_ptr<LogPolicy> policy)
    {
        _strategy = std::move(policy);
    }

    // 创建日志消息的便捷方法
    class LogMessage
    {
    public:
        LogMessage(LogLevel level, const std::string &filename, int line, Logger &logger)
            : _level(level),
              _filename(filename),
              _line(line),
              _logger(logger)
        {
            std::stringstream ss;
            ss << "[" << GetCurrTime() << "] "
               << "[" << EventToString(level) << "] "
               << "[" << filename << ":" << line << "] ";
            _loginfo = ss.str();
        }

        ~LogMessage()
        {
            if (_logger._strategy)
            {
                _logger._strategy->synclog(_loginfo);
            }
        }

        // 流式输出操作符
        template <typename T>
        LogMessage &operator<<(const T &info)
        {
            std::stringstream ss;
            ss << info;
            _loginfo += ss.str();
            return *this;
        }

    private:
        LogLevel _level;
        std::string _filename;
        int _line;
        Logger &_logger;
        std::string _loginfo;
    };

    // 创建日志消息
    LogMessage log(LogLevel level, const std::string &filename, int line)
    {
        return LogMessage(level, filename, line, *this);
    }

private:
    std::unique_ptr<LogPolicy> _strategy;
};

// 全局日志器实例
static Logger g_logger;

#define LOG(level) g_logger.log(level, __FILE__, __LINE__)
