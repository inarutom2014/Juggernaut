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

const size_t COPY_BLOCK_SIZE = 1024 * 1024;//小于1M的直接拷贝
const size_t FILE_SIZE_COMPRESS_MAX = 1024 * 1024 * 100;//可压缩的文件上限 对于特别大的文件 不能一次性全部压缩 因为内存不够用

namespace jugg
{
namespace common
{

int ZipUtility::Compress(unsigned char *dest, size_t *destLen, const unsigned char *source, size_t sourceLen, int level)
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

int ZipUtility::Uncompress(unsigned char *dest, size_t *destLen, const unsigned char *source, size_t sourceLen)
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
        fprintf(stderr, "Open local file %s fail\n", sourceFilePath);
        return ZipResult_FILE_NOT_FOUND;
    }
    struct stat statbuf;
    stat(sourceFilePath, &statbuf);
    int size = statbuf.st_size;
    if (size > FILE_SIZE_COMPRESS_MAX)
    {
        fprintf(stderr, "file %s is too big, don't support compress for now\n", sourceFilePath);
        close(sourcefp);
        return ZipResult_FILE_TOO_BIG;
    }
    DataBlock block(size);
    ssize_t readBytes = read(sourcefp, (void *)block.data(), size);
    close(sourcefp);
    if (readBytes == -1)
    {
        close(sourcefp);
        return ZipResult_READ_FAILURE;
    }

    int destFp = open(destFilePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (destFp == -1)
    {
        fprintf(stderr, "Create local file %s fail\n", destFilePath);
        return ZipResult_CREATE_FILE_FAILURE;
    }
    Config *config = Config::getInstance();
    size_t readBytes_le = readBytes;
        //如果是大端 转化为小端 然后写进文件
    if (config->isBigEndian())
    {
        readBytes_le = BSWAP_32(readBytes);
    }
    if (write(destFp, &readBytes_le, sizeof(readBytes_le)) == -1)
    {
        close(destFp);
        return ZipResult_WRITE_FAILURE;
    }
    DataBlock compressBlock(size);
    size_t compressSize = readBytes;
    if (ZipUtility::Compress((unsigned char *)compressBlock.data(), &compressSize
        ,(const unsigned char *)block.data(), readBytes, compressLevel) != ZipResult_OK)
    {
        close(destFp);
        fprintf(stderr, "Compress  fail\n");
        return ZipResult_Compress_FAILURE;
    }
    if (write(destFp, compressBlock.data(), compressSize) == -1)
    {
        close(destFp);
        return ZipResult_WRITE_FAILURE;
    }
    close(destFp);
    return ZipResult_OK;
}

int ZipUtility::UncompressFile(const char *sourceFilePath, const char *destFilePath)
{
    int sourcefp = open(sourceFilePath, O_RDONLY);
    if (sourcefp == -1)
    {
        fprintf(stderr, "Open local file %s fail\n", sourceFilePath);
        return ZipResult_FILE_NOT_FOUND;
    }
    struct stat statbuf;
    stat(sourceFilePath, &statbuf);
    size_t fileSize = statbuf.st_size;
    DataBlock block(fileSize);

    size_t orignalSize = 0;
    if (read(sourcefp, &orignalSize, sizeof(orignalSize)) == -1)
    {
        close(sourcefp);
        return ZipResult_READ_FAILURE;
    }
    Config *config = Config::getInstance();
    //如果是大端 转化为小端
    if (config->isBigEndian())
    {
        orignalSize = BSWAP_32(orignalSize);
    }

    ssize_t readBytes = read(sourcefp, (void *)block.data(), fileSize);
    if (readBytes == -1)
    {
        close(sourcefp);
        return ZipResult_READ_FAILURE;
    }
    close(sourcefp);

    int destFp = open(destFilePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (destFp == -1)
    {
        fprintf(stderr, "Create local file %s fail\n", destFilePath);
        return ZipResult_CREATE_FILE_FAILURE;
    }
    DataBlock uncompressBlock(orignalSize);
    if(Uncompress((unsigned char *)uncompressBlock.data(), &orignalSize, (unsigned char *)block.data(), readBytes) != ZipResult_OK)
    {
        fprintf(stderr, "Uncompress file %s fail\n", sourceFilePath);
        return 0;
    }
    ssize_t writeBytes = write(destFp, uncompressBlock.data(), orignalSize);
    if (writeBytes == -1)
    {
        close(destFp);
        return ZipResult_WRITE_FAILURE;
    }
    close(destFp);
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