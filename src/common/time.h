#pragma once

#include <sys/time.h>
#include <time.h>
#include <string>

namespace jugg
{
namespace common
{

namespace timer
{

enum Precision
{
    eDay,
    eMin,
    eUsec,
};

static inline long get_micros()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<long>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

static inline int32_t now_time()
{
    return static_cast<int32_t>(get_micros() / 1000000);
}

static inline int32_t now_time_str(char *buf, int32_t len, Precision p = eUsec)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    const time_t seconds = tv.tv_sec;
    struct tm t;
    localtime_r(&seconds, &t);
    int32_t ret = 0;
    if (p == eDay)
    {
        ret = snprintf(buf, len, "%02d/%02d",
                       t.tm_mon + 1,
                       t.tm_mday);
    }
    else if (p == eMin)
    {
        ret = snprintf(buf, len, "%02d/%02d %02d:%02d",
                       t.tm_mon + 1,
                       t.tm_mday,
                       t.tm_hour,
                       t.tm_min);
    }
    else
    {
        ret = snprintf(buf, len, "%02d/%02d %02d:%02d:%02d.%06d",
                       t.tm_mon + 1,
                       t.tm_mday,
                       t.tm_hour,
                       t.tm_min,
                       t.tm_sec,
                       static_cast<int>(tv.tv_usec));
    }
    return ret;
}

}
}
}


