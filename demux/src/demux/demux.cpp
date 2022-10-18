#include "demux.h"
#include "FLVHeader.h"
#include "readStream.h"
#include "FLVFileBody.h"

#define min(a, b) ( (a) < (b) ? (a) : (b) )

static inline uint32_t bswap_32(uint32_t x) {
    x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
    return (x >> 16) | (x << 16);
}

Demux::Demux(const char *filename)
        : filename(filename) {


}

int Demux::init() {

    fs.open(filename, std::ios::out | std::ios::binary);

    if (!fs.is_open()) {
        fprintf(stderr, "open %s failed\n", filename);
        return -1;
    }
    //buffer = new uint8_t[READ_MIN_BYTE_SIZE];

    /*fs.read(reinterpret_cast<char *>(buffer), READ_MIN_BYTE_SIZE);
    readFileSize = fs.gcount();*/

    return 0;
}


int Demux::demux() {

    /*header 9字节*/
    uint8_t buffer1[9];
    fs.read(reinterpret_cast<char *>(buffer1), 9);
    ReadStream rs(buffer1, 9);
    FLVHeader header;
    header.parse(rs);
    // advanceBuffer(header.getDataLength());
    uint8_t tagSize[4]{0};


    std::ofstream videoOutFile("resource/test.h264", std::ios::binary | std::ios::out | std::ios::trunc);
    if (!videoOutFile.is_open()) {
        fprintf(stderr, "open file failed\n");
        return -1;
    }
    std::ofstream audioOutFile("resource/test.aac", std::ios::binary | std::ios::out | std::ios::trunc);
    if (!videoOutFile.is_open()) {
        fprintf(stderr, "open file failed\n");
        return -1;
    }
    while (true) {

        fs.read(reinterpret_cast<char *>(tagSize), 4);

        uint32_t PreviousTagSize = bswap_32(*(reinterpret_cast<uint32_t *>(&tagSize[0])));
        if (fs.peek() == EOF) {
            break;
        }

        FLVFileBody body;
        body.parseHeader(fs, videoOutFile, audioOutFile);

    }
    videoOutFile.close();
    audioOutFile.close();
    return 0;
}

int Demux::fillBuffer() {
    /*每次循环已读的字节数目*/
    if (fillByteSize > 0) {
        /*每次循环还剩多少字节未读，未读的要放到前面来*/
        if (readFileSize) {
            //由src所指内存区域复制count个字节到dest所指内存区域。
            //void *memmove(void *dest, const void *src, unsigned int count);

            /*把后面的未读取的移动到前面来*/
            memmove(buffer, (buffer + fillByteSize), readFileSize);
        }

        if (isEof) {
            /*从文件填充上个循环已经读取的字节数*/
            fs.read(reinterpret_cast<char *>(buffer + readFileSize), fillByteSize);
            uint32_t size = fs.gcount();

            /*如果读不满 fillByteSize 大小代表读到尾部了，size是实际读取了多大*/
            if (size != fillByteSize) {
                isEof = false;
            }
            readFileSize += size;
        }
        fillByteSize = 0;
    }
    return 0;
}

int Demux::advanceBuffer(uint32_t length) {
    if (length > 0 && readFileSize > 0) {
        /*
         * 这里有可能读取文件末尾的时候.
         * length就可能比readFileSize大
         * */
        uint32_t size = min(length, readFileSize);
        fillByteSize += size;
        readFileSize -= size;
    }
    return 0;
}
