#include "sha1.h"
#include "missing.h"
#include "util.h"
#include <string.h>
#include <sstream>
#include <iomanip>

#define ROL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define ROR(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

namespace jugg
{
namespace common
{
SHA_Context::SHA_Context()
{
    reset();
}

void SHA_Context::transform(const unsigned char *data)
{
    uint32_t a, b, c, d, e, tm, x[16];

    /* get values from the chaining vars */
    a = state_[0];
    b = state_[1];
    c = state_[2];
    d = state_[3];
    e = state_[4];

    unsigned k;
    for (k = 0; k < 16; k += 4)
    {
        const unsigned char *p2 = data + k * 4;
        x[k] = util::DecodeBigEndian32((const char *)p2);
        x[k + 1] = util::DecodeBigEndian32((const char *)p2 + 4);
        x[k + 2] = util::DecodeBigEndian32((const char *)p2 + 8);
        x[k + 3] = util::DecodeBigEndian32((const char *)p2 + 12);
    }

#define K1 0x5A827999L
#define K2 0x6ED9EBA1L
#define K3 0x8F1BBCDCL
#define K4 0xCA62C1D6L
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) (x ^ y ^ z)
#define F3(x, y, z) ((x & y) | (z & (x | y)))
#define F4(x, y, z) (x ^ y ^ z)

#define M(i) (tm = x[i & 0x0f] ^ x[(i - 14) & 0x0f] ^ x[(i - 8) & 0x0f] ^ x[(i - 3) & 0x0f], (x[i & 0x0f] = ROL(tm, 1)))

#define SHA1STEP(a, b, c, d, e, f, k, m)     \
    do                                       \
    {                                        \
        e += ROL(a, 5) + f(b, c, d) + k + m; \
        b = ROL(b, 30);                      \
    } while (0)
    SHA1STEP(a, b, c, d, e, F1, K1, x[0]);
    SHA1STEP(e, a, b, c, d, F1, K1, x[1]);
    SHA1STEP(d, e, a, b, c, F1, K1, x[2]);
    SHA1STEP(c, d, e, a, b, F1, K1, x[3]);
    SHA1STEP(b, c, d, e, a, F1, K1, x[4]);
    SHA1STEP(a, b, c, d, e, F1, K1, x[5]);
    SHA1STEP(e, a, b, c, d, F1, K1, x[6]);
    SHA1STEP(d, e, a, b, c, F1, K1, x[7]);
    SHA1STEP(c, d, e, a, b, F1, K1, x[8]);
    SHA1STEP(b, c, d, e, a, F1, K1, x[9]);
    SHA1STEP(a, b, c, d, e, F1, K1, x[10]);
    SHA1STEP(e, a, b, c, d, F1, K1, x[11]);
    SHA1STEP(d, e, a, b, c, F1, K1, x[12]);
    SHA1STEP(c, d, e, a, b, F1, K1, x[13]);
    SHA1STEP(b, c, d, e, a, F1, K1, x[14]);
    SHA1STEP(a, b, c, d, e, F1, K1, x[15]);
    SHA1STEP(e, a, b, c, d, F1, K1, M(16));
    SHA1STEP(d, e, a, b, c, F1, K1, M(17));
    SHA1STEP(c, d, e, a, b, F1, K1, M(18));
    SHA1STEP(b, c, d, e, a, F1, K1, M(19));
    SHA1STEP(a, b, c, d, e, F2, K2, M(20));
    SHA1STEP(e, a, b, c, d, F2, K2, M(21));
    SHA1STEP(d, e, a, b, c, F2, K2, M(22));
    SHA1STEP(c, d, e, a, b, F2, K2, M(23));
    SHA1STEP(b, c, d, e, a, F2, K2, M(24));
    SHA1STEP(a, b, c, d, e, F2, K2, M(25));
    SHA1STEP(e, a, b, c, d, F2, K2, M(26));
    SHA1STEP(d, e, a, b, c, F2, K2, M(27));
    SHA1STEP(c, d, e, a, b, F2, K2, M(28));
    SHA1STEP(b, c, d, e, a, F2, K2, M(29));
    SHA1STEP(a, b, c, d, e, F2, K2, M(30));
    SHA1STEP(e, a, b, c, d, F2, K2, M(31));
    SHA1STEP(d, e, a, b, c, F2, K2, M(32));
    SHA1STEP(c, d, e, a, b, F2, K2, M(33));
    SHA1STEP(b, c, d, e, a, F2, K2, M(34));
    SHA1STEP(a, b, c, d, e, F2, K2, M(35));
    SHA1STEP(e, a, b, c, d, F2, K2, M(36));
    SHA1STEP(d, e, a, b, c, F2, K2, M(37));
    SHA1STEP(c, d, e, a, b, F2, K2, M(38));
    SHA1STEP(b, c, d, e, a, F2, K2, M(39));
    SHA1STEP(a, b, c, d, e, F3, K3, M(40));
    SHA1STEP(e, a, b, c, d, F3, K3, M(41));
    SHA1STEP(d, e, a, b, c, F3, K3, M(42));
    SHA1STEP(c, d, e, a, b, F3, K3, M(43));
    SHA1STEP(b, c, d, e, a, F3, K3, M(44));
    SHA1STEP(a, b, c, d, e, F3, K3, M(45));
    SHA1STEP(e, a, b, c, d, F3, K3, M(46));
    SHA1STEP(d, e, a, b, c, F3, K3, M(47));
    SHA1STEP(c, d, e, a, b, F3, K3, M(48));
    SHA1STEP(b, c, d, e, a, F3, K3, M(49));
    SHA1STEP(a, b, c, d, e, F3, K3, M(50));
    SHA1STEP(e, a, b, c, d, F3, K3, M(51));
    SHA1STEP(d, e, a, b, c, F3, K3, M(52));
    SHA1STEP(c, d, e, a, b, F3, K3, M(53));
    SHA1STEP(b, c, d, e, a, F3, K3, M(54));
    SHA1STEP(a, b, c, d, e, F3, K3, M(55));
    SHA1STEP(e, a, b, c, d, F3, K3, M(56));
    SHA1STEP(d, e, a, b, c, F3, K3, M(57));
    SHA1STEP(c, d, e, a, b, F3, K3, M(58));
    SHA1STEP(b, c, d, e, a, F3, K3, M(59));
    SHA1STEP(a, b, c, d, e, F4, K4, M(60));
    SHA1STEP(e, a, b, c, d, F4, K4, M(61));
    SHA1STEP(d, e, a, b, c, F4, K4, M(62));
    SHA1STEP(c, d, e, a, b, F4, K4, M(63));
    SHA1STEP(b, c, d, e, a, F4, K4, M(64));
    SHA1STEP(a, b, c, d, e, F4, K4, M(65));
    SHA1STEP(e, a, b, c, d, F4, K4, M(66));
    SHA1STEP(d, e, a, b, c, F4, K4, M(67));
    SHA1STEP(c, d, e, a, b, F4, K4, M(68));
    SHA1STEP(b, c, d, e, a, F4, K4, M(69));
    SHA1STEP(a, b, c, d, e, F4, K4, M(70));
    SHA1STEP(e, a, b, c, d, F4, K4, M(71));
    SHA1STEP(d, e, a, b, c, F4, K4, M(72));
    SHA1STEP(c, d, e, a, b, F4, K4, M(73));
    SHA1STEP(b, c, d, e, a, F4, K4, M(74));
    SHA1STEP(a, b, c, d, e, F4, K4, M(75));
    SHA1STEP(e, a, b, c, d, F4, K4, M(76));
    SHA1STEP(d, e, a, b, c, F4, K4, M(77));
    SHA1STEP(c, d, e, a, b, F4, K4, M(78));
    SHA1STEP(b, c, d, e, a, F4, K4, M(79));

#undef F1
#undef F2
#undef F3
#undef F4

    /* Update chaining vars */
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
}

void SHA_Context::write(const unsigned char *buf, size_t len)
{
    size_t num = this->bytecount_ & 0x3f;

    /* Update bytecount */
    this->bytecount_ += len;

    /* Handle any leading odd-sized chunks */
    if (num)
    {
        unsigned char *p = this->buf_ + num;

        num = 64 - num;
        if (len < num)
        {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, num);
        transform(this->buf_);
        buf += num;
        len -= num;
    }

    /* Process data in 64-byte chunks */
    while (len >= 64)
    {
        PREFETCH64(buf + 64);
        transform(buf);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */
    memcpy(this->buf_, buf, len);
}

void SHA_Context::write(const std::string& content)
{
    const unsigned char *buf = (const unsigned char *)content.c_str();
    size_t len = content.length();
    write(buf, len);
}

void SHA_Context::final()
{
    uint64_t bitcount = this->bytecount_ << 3;
    size_t pos = ((size_t)this->bytecount_) & 0x3F;
    unsigned char *p;

    this->buf_[pos++] = 0x80;

    /* Pad whatever data is left in the buffer */
    while (pos != (64 - 8))
    {
        pos &= 0x3F;
        if (pos == 0)
            transform(this->buf_);
        this->buf_[pos++] = 0;
    }

    /* Append to the padding the total message's length in bits and transform */
    this->buf_[63] = (unsigned char)bitcount;
    this->buf_[62] = (unsigned char)(bitcount >> 8);
    this->buf_[61] = (unsigned char)(bitcount >> 16);
    this->buf_[60] = (unsigned char)(bitcount >> 24);
    this->buf_[59] = (unsigned char)(bitcount >> 32);
    this->buf_[58] = (unsigned char)(bitcount >> 40);
    this->buf_[57] = (unsigned char)(bitcount >> 48);
    this->buf_[56] = (unsigned char)(bitcount >> 56);

    transform(this->buf_);

    p = this->buf_;

    util::EncodeBigEndian((char *)p, this->state_[0]);
    p += 4;
    util::EncodeBigEndian((char *)p, this->state_[1]);
    p += 4;
    util::EncodeBigEndian((char *)p, this->state_[2]);
    p += 4;
    util::EncodeBigEndian((char *)p, this->state_[3]);
    p += 4;
    util::EncodeBigEndian((char *)p, this->state_[4]);
    p += 4;
}

void SHA_Context::reset()
{
    memset(this, 0, sizeof(SHA_Context));
    state_[0] = 0x67452301;
    state_[1] = 0xEFCDAB89;
    state_[2] = 0x98BADCFE;
    state_[3] = 0x10325476;
    state_[4] = 0xC3D2E1F0;
}

std::string SHA_Context::getSHA1()
{
    std::ostringstream oss;
    for (int i = 0; i < 20; i++)
    {
        unsigned char ch = buf_[i];
        oss << std::hex
            << std::setw(2) << std::setfill('0')
            << static_cast<int>(ch);
    }
    return oss.str();
}
}
}