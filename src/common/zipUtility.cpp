#include "zipUtility.h"
#include "dataBlock.h"
#include "missing.h"
#include "../config/configuration.h"
#include "logging.h"
#include <zlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>

const uint32_t COPY_BLOCK_SIZE = 1024 * 1024;//小于1M的直接拷贝
const uint32_t COMPRESS_BLOCK_SIZE = 1024 * 1024 * 2;   //以1M为单位压缩文件
const uint64_t FILE_SIZE_COMPRESS_MAX = 0x00000004L << 30;//可压缩的文件上限

namespace jugg
{
namespace common
{

ssize_t readUint32(int fp, uint32_t *out)
{
    uint32_t readValue = 0;
    ssize_t result = read(fp, &readValue, sizeof(uint32_t));
    if (result == -1)
    {
        *out = 0;
        return result;
    }
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        write_swap32(out, readValue);
    }else
    {
        *out = readValue;
    }
    return result;
}

ssize_t readInt32(int fp, int32_t *out)
{
    int32_t readValue = 0;
    ssize_t result = read(fp, &readValue, sizeof(int32_t));
    if (result == -1)
    {
        *out = 0;
        return result;
    }
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        write_swap32(out, readValue);
    }
    else
    {
        *out = readValue;
    }
    return result;
}

ssize_t readInt64(int fp, int64_t *out)
{
    int64_t readValue = 0;
    ssize_t result = read(fp, &readValue, sizeof(int64_t));
    if (result == -1)
    {
        *out = 0;
        return result;
    }
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        write_swap64(out, readValue);
    }
    else
    {
        *out = readValue;
    }
    return result;
}

ssize_t readUInt64(int fp, uint64_t *out)
{
    uint64_t readValue = 0;
    ssize_t result = read(fp, &readValue, sizeof(uint64_t));
    if (result == -1)
    {
        *out = 0;
        return result;
    }
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        write_swap64(out, readValue);
    }
    else
    {
        *out = readValue;
    }
    return result;
}

ssize_t writeUint32(int fp, uint32_t v)
{
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        BSWAP_32(v);
    }
    return write(fp, &v, sizeof(uint32_t));
}

ssize_t writeInt32(int fp, int32_t v)
{
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        BSWAP_32(v);
    }
    return write(fp, &v, sizeof(int32_t));
}

ssize_t writeInt64(int fp, int64_t v)
{
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        BSWAP_64(v);
    }
    return write(fp, &v, sizeof(int64_t));
}

ssize_t writeUint64(int fp, uint64_t v)
{
    Config *config = Config::getInstance();
    if (config->isBigEndian())
    {
        BSWAP_64(v);
    }
    return write(fp, &v, sizeof(uint64_t));
}

void ZipInfoHeader_init(ZipInfoHeader *header, uint64_t orignalSize)
{
    header->orignalSize = orignalSize;
    if (orignalSize > COMPRESS_BLOCK_SIZE)
    {
        header->blockSize = COMPRESS_BLOCK_SIZE;
        header->blockNum = orignalSize / COMPRESS_BLOCK_SIZE;
        if (header->blockNum * COMPRESS_BLOCK_SIZE < orignalSize)
        {
            header->blockNum++;
        }
    }
    else
    {
        header->blockSize = orignalSize;
        header->blockNum = 1;
    }
}

void ZipInfoHeader_write(ZipInfoHeader *header, int fp)
{
    writeUint64(fp, header->orignalSize);
    writeUint32(fp, header->blockSize);
    writeUint32(fp, header->blockNum);
}

void ZipInfoHeader_read(ZipInfoHeader *header, int fp)
{
    readUInt64(fp, &(header->orignalSize));
    readUint32(fp, &(header->blockSize));
    readUint32(fp, &(header->blockNum));
}

void ZipBlockInfoHeader_write(ZipBlockInfoHeader *header, int fp)
{
    writeUint32(fp, header->orignalSize);
    writeUint32(fp, header->compressSize);
}

void ZipBlockInfoHeader_read(ZipBlockInfoHeader *header, int fp)
{
    readUint32(fp, &(header->orignalSize));
    readUint32(fp, &(header->compressSize));
}

int ZipUtility::Compress(unsigned char *dest, uint32_t *destLen, const unsigned char *source, uint32_t sourceLen, int level)
{
    z_stream stream;
    int err;
    const uInt max = (uInt)-1;
    uLong left;

    left = *destLen;
    *destLen = 0;

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    err  = deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    if (err != Z_OK)
        return err;

    stream.next_out = dest;
    stream.avail_out = 0;
    stream.next_in = (z_const Bytef *)source;
    stream.avail_in = 0;

    do
    {
        if (stream.avail_out == 0)
        {
            stream.avail_out = left > (uLong)max ? max : (uInt)left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0)
        {
            stream.avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
            sourceLen -= stream.avail_in;
        }
        err = deflate(&stream, sourceLen ? Z_NO_FLUSH : Z_FINISH);
    } while (err == Z_OK);

    *destLen = stream.total_out;
    deflateEnd(&stream);
    return err == Z_STREAM_END ? ZipResult_OK : ZipResult_Error;
}

int ZipUtility::Uncompress(unsigned char *dest, uint32_t *destLen, const unsigned char *source, uint32_t sourceLen)
{
    z_stream stream;
    int err;
    const uInt max = (uInt)-1;
    uLong len, left;
    Byte buf[1]; /* for detection of incomplete stream when *destLen == 0 */

    len = sourceLen;
    if (*destLen)
    {
        left = *destLen;
        *destLen = 0;
    }
    else
    {
        left = 1;
        dest = buf;
    }

    stream.next_in = (z_const Bytef *)source;
    stream.avail_in = 0;
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = inflateInit2(&stream, -MAX_WBITS);
    if (err != Z_OK)
        return err;

    stream.next_out = dest;
    stream.avail_out = 0;

    do
    {
        if (stream.avail_out == 0)
        {
            stream.avail_out = left > (uLong)max ? max : (uInt)left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0)
        {
            stream.avail_in = len > (uLong)max ? max : (uInt)len;
            len -= stream.avail_in;
        }
        err = inflate(&stream, Z_NO_FLUSH);
    } while (err == Z_OK);

    if (dest != buf)
        *destLen = stream.total_out;
    else if (stream.total_out && err == Z_BUF_ERROR)
        left = 1;

    inflateEnd(&stream);
    return err == Z_STREAM_END ? Z_OK : err == Z_NEED_DICT ? Z_DATA_ERROR : err == Z_BUF_ERROR && left + stream.avail_out ? Z_DATA_ERROR : err;
}

int ZipUtility::CompressFile(const char *sourceFilePath, const char *destFilePath, int compressLevel)
{
    int sourcefp = open(sourceFilePath, O_RDONLY);
    if (sourcefp == -1)
    {
        LOG_ERROR("Open local file %s fail", sourceFilePath);
        return ZipResult_FILE_NOT_FOUND;
    }
    struct stat statbuf;
    stat(sourceFilePath, &statbuf);
    uint64_t size = statbuf.st_size;
    if (size > FILE_SIZE_COMPRESS_MAX)
    {
        LOG_ERROR("file %s is too big, don't support compress for now\n", sourceFilePath);
        close(sourcefp);
        return ZipResult_FILE_TOO_BIG;
    }

    int destFp = open(destFilePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (destFp == -1)
    {
        LOG_ERROR("Create local file %s fail\n", destFilePath);
        return ZipResult_CREATE_FILE_FAILURE;
    }

    ZipInfoHeader zipInfoHeader;
    ZipInfoHeader_init(&zipInfoHeader, size);
    ZipInfoHeader_write(&zipInfoHeader, destFp);
    uint32_t blockSize = zipInfoHeader.blockSize;
    DataBlock block(blockSize);
    DataBlock compressBlock(blockSize);
    ZipBlockInfoHeader blockHeader;
    LOG_DEBUG("compress begin");
    for (int i = 0; i < zipInfoHeader.blockNum; i++)
    {
        ssize_t readBytes = read(sourcefp, (void *)block.data(), blockSize);
        if (readBytes == -1)
        {
            close(sourcefp);
            close(destFp);
            unlink(destFilePath);
            LOG_ERROR("read fail");
            return ZipResult_READ_FAILURE;
        }

        blockHeader.orignalSize = readBytes;
        blockHeader.compressSize = readBytes;
        if (ZipUtility::Compress((unsigned char *)compressBlock.data(), &(blockHeader.compressSize), (const unsigned char *)block.data(), readBytes, compressLevel) != ZipResult_OK)
        {
            close(sourcefp);
            close(destFp);
            unlink(destFilePath);
            LOG_ERROR("compress  fail");
            return ZIPRESULT_COMPRESS_FAILURE;
        }
        ZipBlockInfoHeader_write(&blockHeader, destFp);
        if (write(destFp, compressBlock.data(), blockHeader.compressSize) == -1)
        {
            close(sourcefp);
            close(destFp);
            unlink(destFilePath);
            LOG_ERROR("write fail");
            return ZipResult_WRITE_FAILURE;
        }
        LOG_DEBUG("compress process %f%%", (float)i / (float)zipInfoHeader.blockNum * 100);
    }
    close(sourcefp);
    close(destFp);
    LOG_DEBUG("compress complete");
    return ZipResult_OK;
}

int ZipUtility::UncompressFile(const char *sourceFilePath, const char *destFilePath)
{
    int sourceFp = open(sourceFilePath, O_RDONLY);
    if (sourceFp == -1)
    {
        LOG_ERROR("Open local file %s fail\n", sourceFilePath);
        return ZipResult_FILE_NOT_FOUND;
    }
    int destFp = open(destFilePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (destFp == -1)
    {
        close(sourceFp);
        LOG_ERROR("Create local file %s fail\n", destFilePath);
        return ZipResult_CREATE_FILE_FAILURE;
    }
    ZipInfoHeader zipInfoHeader;
    ZipInfoHeader_read(&zipInfoHeader, sourceFp);
    ZipBlockInfoHeader blockInfoHeader;
    DataBlock block(zipInfoHeader.blockSize);
    DataBlock uncompressBlock(zipInfoHeader.blockSize);
    LOG_DEBUG("uncompress begin");
    for (int i = 0; i < zipInfoHeader.blockNum; i++)
    {
        ZipBlockInfoHeader_read(&blockInfoHeader, sourceFp);
        ssize_t readBytes = read(sourceFp, (void *)block.data(), blockInfoHeader.compressSize);
        if (readBytes == -1)
        {
            close(sourceFp);
            close(destFp);
            unlink(destFilePath);
            LOG_ERROR("Read file %s fail\n", sourceFilePath);
            return ZipResult_READ_FAILURE;
        }
        if (Uncompress((unsigned char *)uncompressBlock.data(), &blockInfoHeader.orignalSize, (unsigned char *)block.data(), readBytes) != ZipResult_OK)
        {
            close(sourceFp);
            close(destFp);
            unlink(destFilePath);
            LOG_ERROR("Uncompress file %s fail\n", sourceFilePath);
            return ZIPRESULT_UNCOMPRESS_FAILURE;
        }
        if (write(destFp, uncompressBlock.data(), blockInfoHeader.orignalSize) == -1)
        {
            close(sourceFp);
            close(destFp);
            unlink(destFilePath);
            LOG_ERROR("Write file %s fail", destFilePath);
            return ZipResult_WRITE_FAILURE;
        }
        LOG_DEBUG("uncompress process %f%%", (float)i / (float)zipInfoHeader.blockNum * 100);
    }
    close(sourceFp);
    close(destFp);
    LOG_DEBUG("compress complete");
    return ZipResult_OK;
}

int ZipUtility::CopyFile(const char *sourceFilePath, const char *destFilePath)
{
    int sourceFp = open(sourceFilePath, O_RDONLY);
    if (sourceFp == -1)
    {
        fprintf(stderr, "Open local file %s fail\n", sourceFilePath);
        return ZipResult_FILE_NOT_FOUND;
    }
    int destFp = open(destFilePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (destFp == -1)
    {
        close(sourceFp);
        fprintf(stderr, "Create local file %s fail\n", destFilePath);
        return ZipResult_CREATE_FILE_FAILURE;
    }
    struct stat statbuf;
    stat(sourceFilePath, &statbuf);
    int size = statbuf.st_size;
    size_t buffLen = size < COPY_BLOCK_SIZE ? size : COPY_BLOCK_SIZE;
    DataBlock block(buffLen);
    ssize_t readBytes = 0;
    while ((readBytes = read(sourceFp, (void *)block.data(), buffLen)) > 0)
    {
        if (write(destFp, block.data(), readBytes) != readBytes)
        {
            close(sourceFp);
            close(destFp);
            fprintf(stderr, "Write file %s fail\n", destFilePath);
            return ZipResult_WRITE_FAILURE;
        }
    }
    close(sourceFp);
    close(destFp);
    return ZipResult_OK;
}


}
}