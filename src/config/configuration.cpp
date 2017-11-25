#include "configuration.h"

namespace Jugg {
Config *Config::instance_s = nullptr;

Config *Config::getInstance()
{
    if (instance_s == nullptr)
    {
        instance_s = new Config();
        instance_s->init();
    }
    return instance_s;
}

void Config::init()
{
    checkEndian();
}

void Config::checkEndian()
{
    unsigned short test = 0x1122;
    if (*(unsigned char*)&test == 0x11)
        isBigEndian_ = true;
    else
        isBigEndian_ = false;
}


}