//移植于https://github.com/pbatard/rufus
//针对一些目标的平台和功能需求做了一些修改
#pragma once

//预抓取 把数据放在最近的内存中 减少访问延时
#if defined(__GNUC__) || defined(__clang__)
#define PREFETCH64(m)                     \
    do                                    \
    {                                     \
        __builtin_prefetch(m, 0, 0);      \
        __builtin_prefetch(m + 32, 0, 0); \
    } while (0)
#elif defined(_MSC_VER)
#if defined(_M_IX86) || defined(_M_X64)
#define PREFETCH64(m)        \
    do                       \
    {                        \
        _m_prefetch(m);      \
        _m_prefetch(m + 32); \
    } while (0)
#else
// _m_prefetch() doesn't seem to exist for MSVC/ARM
#define PREFETCH64(m)
#endif
#endif

//字节码高低位交换
#define BSWAP_8(x) ((x)&0xff)
#define BSWAP_16(x) ((BSWAP_8(x) << 8) | BSWAP_8((x) >> 8))
#define BSWAP_32(x) ((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define BSWAP_64(x) ((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))
#define read_swap16(p) BSWAP_16(*(const uint16_t *)(const uint8_t *)(p))
#define read_swap32(p) BSWAP_32(*(const uint32_t *)(const uint8_t *)(p))
#define read_swap64(p) BSWAP_64(*(const uint64_t *)(const uint8_t *)(p))
#define write_swap16(p, v) (*(uint16_t *)(void *)(p)) = BSWAP_16(v)
#define write_swap32(p, v) (*(uint32_t *)(void *)(p)) = BSWAP_32(v)
#define write_swap64(p, v) (*(uint32_t *)(void *)(p)) = BSWAP_64(v)