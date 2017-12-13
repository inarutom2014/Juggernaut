#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>
namespace jugg
{
namespace common
{
    class Md5_Context{
    public:
        Md5_Context();
        void write(const unsigned char *buf, size_t len);
        void write(const std::string &content);
        void final();
        std::string getMd5();
        void reset();
    private:
        void transform(const unsigned char *data);
    private: 
        Md5_Context(const Md5_Context &);   //no copy
        Md5_Context &operator=(const Md5_Context &);    //no assign
    private:
        unsigned char buf_[64];
        uint32_t state_[8];
        uint64_t bytecount_;
    };

}
}