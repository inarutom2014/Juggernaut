//author: lishuai
//desc: 用于计算SHA1值
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>
namespace jugg 
{
namespace common
{
    class SHA_Context
    {
      public:
        SHA_Context();
        void write(const unsigned char *buf, size_t len);
        void write(const std::string &content);
        void final();
        void reset();
        std::string getSHA1();
      private: 
        void transform(const unsigned char *data);
      private:
        unsigned char buf_[64];
        uint32_t state_[8];
        uint64_t bytecount_;

      private:
        SHA_Context(const SHA_Context &); //no copy
        SHA_Context& operator=(const SHA_Context&); //no assign
    };
}
}