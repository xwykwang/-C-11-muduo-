#include "Logger.h"
#include "Timestamp.h"

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLeverl lev)
{
    this->loglevel_ = lev;
}

void Logger::log(std::string msg)
{
    switch(loglevel_)
    {
        case(LogLeverl::INFO):
            std::cout<<"[INFO]: ";
            break;
        case(LogLeverl::ERROR):
            std::cout<<"[ERROR]:";
            break;
        case(LogLeverl::FATAL):
            std::cout<<"[FATAL]";
            break;
        case(LogLeverl::DEBUG):
            std::cout<<"[DEBUG]";
            break;
        default:
            break;
    }
    std::cout<<Timestamp::now().toString()<<" "<<msg<<std::endl;
}