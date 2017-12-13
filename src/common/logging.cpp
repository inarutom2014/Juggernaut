#include "logging.h"
#include "time.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
namespace jugg
{
namespace common
{

int g_log_level = INFO;
FILE *g_log_file = stdout;

static const std::string level_str[] = {
    "Verbose", "Debug", "Info", "Warning", "Error", "Fatal"};

void SetLogLevel(int level)
{
    g_log_level = level;
}

bool SetLogFile(const char *path, bool append)
{
    const char *mode = append ? "ab" : "wb";
    FILE *fp = fopen(path, mode);
    if (fp == NULL)
    {
        return false;
    }
    if (g_log_file)
    {
        fclose(g_log_file);
    }
    g_log_file = fp;
    return true;
}

void Logv(int level, const char *fmt, va_list ap)
{
    char buffer[1024];
    char *p = buffer;

    // static const char level_char[] = {
    //     'V', 'D', 'I', 'W', 'E', 'F'};
    // char cur_level = level_char[0];
    int index = 0;
    if (level < DEBUG)
    {
        index = 0;
    }
    else if (level < INFO)
    {
        index = 1;
    }
    else if (level < WARNING)
    {
        index = 2;
    }
    else if (level < ERROR)
    {
        index = 3;
    }
    else if (level < FATAL)
    {
        // cur_level = level_char[4];
        index = 4;
    }
    else
    {
        // cur_level = level_char[5];
        index = 5;
    }
    char *limit = buffer + 1024;
    const std::string &levelstr = level_str[index];
    strcpy(p, levelstr.c_str());
    p += levelstr.length() + 1;
    *p++ = ' ';

    int32_t timeLen = timer::now_time_str(p, limit-p);
    p+=timeLen;
    if (p < limit)
    {
        va_list backup_ap;
        va_copy(backup_ap, ap);
        p += vsnprintf(p, 1024 - timeLen, fmt, backup_ap);
        va_end(backup_ap);
    }

    if (p == buffer || p[-1] != '\n')
    {
        *p++ = '\n';
    }
    fwrite(buffer, p - buffer, 1, g_log_file);
    fflush(g_log_file);
}

void Log(int level, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    if (level >= g_log_level)
    {
        Logv(level, fmt, ap);
    }
    va_end(ap);
    if (level == FATAL)
    {
        abort();
    }
}

}
}