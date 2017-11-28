#pragma once
#include <cstddef>

namespace Jugg
{
namespace common
{
    class ZipUtility
    {
    public:
        static int compress(unsigned char *dest, size_t *destLen, const unsigned char *source, size_t sourceLen, int level);
        static int uncompress(unsigned char *dest, size_t *destLen, const unsigned char *source, size_t sourceLen);
    };
}
}