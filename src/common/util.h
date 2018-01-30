#ifndef JUGG_COMMON_UTIL_H_
#define JUGG_COMMON_UTIL_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <vector>

namespace jugg {
namespace common {
namespace util {

static inline void EncodeLittleEndian(char *buf, uint32_t value)
{
    buf[0] = value & 0xff;
    buf[1] = (value >> 8) & 0xff;
    buf[2] = (value >> 16) & 0xff;
    buf[3] = (value >> 24) & 0xff;
}

static inline void EncodeBigEndian(char *buf, uint32_t value)
{
    buf[0] = (value >> 24) & 0xff;
    buf[1] = (value >> 16) & 0xff;
    buf[2] = (value >> 8) & 0xff;
    buf[3] = value & 0xff;
}

static inline uint32_t DecodeBigEndian32(const char *buf)
{
    return ((static_cast<uint64_t>(static_cast<unsigned char>(buf[0])) << 24) | (static_cast<uint64_t>(static_cast<unsigned char>(buf[1])) << 16) | (static_cast<uint64_t>(static_cast<unsigned char>(buf[2])) << 8) | (static_cast<uint64_t>(static_cast<unsigned char>(buf[3]))));
}

static inline uint32_t DecodeLittleEndian32(const char *buf)
{
    return ((static_cast<uint64_t>(static_cast<unsigned char>(buf[3])) << 24) | (static_cast<uint64_t>(static_cast<unsigned char>(buf[2])) << 16) | (static_cast<uint64_t>(static_cast<unsigned char>(buf[1])) << 8) | (static_cast<uint64_t>(static_cast<unsigned char>(buf[0]))));
}

} //namespace util
} //namespace common
} //namespace jugg
#endif //JUGG_COMMON_UTIL_H_