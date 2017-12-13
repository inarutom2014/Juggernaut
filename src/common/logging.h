#pragma once

namespace jugg
{
namespace common
{

enum LogLevel
{
    DEBUG = 2,
    INFO = 4,
    WARNING = 8,
    ERROR = 16,
    FATAL = 32,
};

void SetLogLevel(int level);
bool SetLogFile(const char *path, bool append);
void Log(int level, const char *fmt, ...);

}

using common::DEBUG;
using common::ERROR;
using common::FATAL;
using common::INFO;
using common::WARNING;

}

#define LOG(level, fmt, args...) jugg::common::Log(level, "[%s:%d] " fmt, __FILE__, __LINE__, ##args)
#define LOG_ERROR(fmt, args...) LOG(jugg::ERROR, fmt, ##args)
#define LOG_DEBUG(fmt, args...) LOG(jugg::DEBUG, fmt, ##args)
#define LOG_INFO(fmt, args...) LOG(jugg::INFO, fmt, ##args)
#define LOG_WARNING(fmt, args...) LOG(jugg::WARNING, fmt, ##args)
#define LOG_FATAL(fmt, args...) LOG(jugg::FATAL, fmt, ##args)