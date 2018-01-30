#include "md5.h"
#include "missing.h"
#include "util.h"
#include <sstream>
#include <iomanip>

namespace jugg
{
namespace common
{

Md5_Context::Md5_Context()
{
    reset();
}

void Md5_Context::reset()
{
    memset(this, 0, sizeof(Md5_Context));
    state_[0] = 0x67452301;
    state_[1] = 0xefcdab89;
    state_[2] = 0x98badcfe;
    state_[3] = 0x10325476;
}

void Md5_Context::transform(const unsigned char *data)
{
    uint32_t a, b, c, d, x[16];

    a = state_[0];
    b = state_[1];
    c = state_[2];
    d = state_[3];

    unsigned k;
    for (k = 0; k < 16; k += 4)
    {
        const unsigned char *p2 = data + k * 4;
        x[k] = util::DecodeLittleEndian32((const char *)p2);
        x[k + 1] = util::DecodeLittleEndian32((const char *)(p2 + 4));
        x[k + 2] = util::DecodeLittleEndian32((const char *)(p2 + 8));
        x[k + 3] = util::DecodeLittleEndian32((const char *)(p2 + 12));
    }

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define MD5STEP(f, w, x, y, z, data, s)                               \
    do                                                                \
    {                                                                 \
        (w += f(x, y, z) + data, w = w << s | w >> (32 - s), w += x); \
    } while (0)

    MD5STEP(F1, a, b, c, d, x[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, x[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, x[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, x[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, x[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, x[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, x[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, x[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, x[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, x[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, x[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, x[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, x[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, x[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, x[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, x[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, x[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, x[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, x[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, x[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, x[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, x[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, x[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, x[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, x[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, x[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, x[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, x[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, x[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, x[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, x[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, x[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, x[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, x[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, x[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, x[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, x[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, x[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, x[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, x[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, x[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, x[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, x[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, x[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, x[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, x[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, x[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, x[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, x[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, x[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, x[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, x[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, x[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, x[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, x[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, x[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, x[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, x[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, x[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, x[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, x[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, x[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, x[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, x[9] + 0xeb86d391, 21);

#undef F1
#undef F2
#undef F3
#undef F4

    /* Update chaining vars */
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
}

void Md5_Context::write(const unsigned char *buf, size_t len)
{
    size_t num = this->bytecount_ & 0x3f;

	/* Update bytecount */
	this->bytecount_ += len;

	/* Handle any leading odd-sized chunks */
	if (num) {
		unsigned char *p = this->buf_ + num;

		num = 64 - num;
		if (len < num) {
			memcpy(p, buf, num);
			return;
		}
		memcpy(p, buf, num);
		transform(this->buf_);
		buf += num;
		len -= num;
	}

	/* Process data in 64-byte chunks */
	while (len >= 64) {
		PREFETCH64(buf + 64);
		transform(buf);
		buf += 64;
		len -= 64;
	}

	/* Handle any remaining bytes of data. */
	memcpy(this->buf_, buf, len);
}

void Md5_Context::write(const std::string &content)
{
    const unsigned char *buf = reinterpret_cast<const unsigned char *>(content.c_str());
    size_t len = content.length();
    write(buf, len);
}

void Md5_Context::final()
{
    size_t count = ((size_t)this->bytecount_) & 0x3F;
    uint64_t bitcount = this->bytecount_ << 3;
    unsigned char *p;

    /* Set the first char of padding to 0x80.
	 * This is safe since there is always at least one byte free
	 */
    p = this->buf_ + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8)
    {
        /* Two lots of padding: Pad the first block to 64 bytes */
        memset(p, 0, count);
        transform(this->buf_);

        /* Now fill the next block with 56 bytes */
        memset(this->buf_, 0, 56);
    }
    else
    {
        /* Pad block to 56 bytes */
        memset(p, 0, count - 8);
    }

    /* append the 64 bit count (little endian) */
    this->buf_[56] = (unsigned char)bitcount;
    this->buf_[57] = (unsigned char)(bitcount >> 8);
    this->buf_[58] = (unsigned char)(bitcount >> 16);
    this->buf_[59] = (unsigned char)(bitcount >> 24);
    this->buf_[60] = (unsigned char)(bitcount >> 32);
    this->buf_[61] = (unsigned char)(bitcount >> 40);
    this->buf_[62] = (unsigned char)(bitcount >> 48);
    this->buf_[63] = (unsigned char)(bitcount >> 56);

    transform(this->buf_);

    p = this->buf_;

    util::EncodeLittleEndian((char *)p, this->state_[0]);
    p += 4;
    util::EncodeLittleEndian((char *)p, this->state_[1]);
    p += 4;
    util::EncodeLittleEndian((char *)p, this->state_[2]);
    p += 4;
    util::EncodeLittleEndian((char *)p, this->state_[3]);
    p += 4;
}

std::string Md5_Context::getMd5()
{
    std::ostringstream oss;
    for (int i = 0; i < 16; i++)
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