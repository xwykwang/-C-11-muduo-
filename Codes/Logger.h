#pragma once

#include <string>
#include <iostream>
#include "noncopyable.h"

#define LOG_INFO(logMsgFormat, ...) \
    do\
    {\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(LogLeverl::INFO);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    } while (0);

#define LOG_ERROR(logMsgFormat, ...) \
    do\
    {\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(LogLeverl::ERROR);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    } while (0);

#define LOG_FATAL(logMsgFormat, ...) \
    do\
    {\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(LogLeverl::FATAL);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
        exit(-1);\
    } while (0);

#ifdef MUDEBUG
#define LOG_DEBUG(logMsgFormat, ...) \
    do\
    {\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(LogLeverl::DEBUG);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    } while (0);
#else 
    #define LOG_DEBUG(logMsgFormat, ...);
#endif

enum class LogLeverl
{
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger:noncopyable
{
public:
    //生成唯一实例对象
    static Logger& instance();
    //设置日志信息级别
    void setLogLevel(LogLeverl lev);
    //打印日志信息
    void log(std::string msg);
private:
    LogLeverl loglevel_;
    Logger(){};
};