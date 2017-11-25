//author: lishuai
//desc: 用于计算SHA1值 参考了GIT中实现
#include <cstdint>
#include <cstddef>
namespace Jugg 
{
namespace common
{
    class SHA_Context
    {
      public:
        SHA_Context();
        void write(const unsigned char *buf, size_t len);
        void final();
      private:
        void transform(const unsigned char* data);
        
      private:
        unsigned char buf_[64];
        uint32_t state_[8];
        uint64_t bytecount_;

      private:
        SHA_Context(const SHA_Context &); //no copy
    };
}
}