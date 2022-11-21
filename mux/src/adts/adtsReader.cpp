#include "adtsReader.h"

#include <cstdio>
#include <cstring>

#include "readStream.h"
#include "adtsHeader.h"

#define MAX_DATA 8191
#define min(a, b) ( (a) < (b) ? (a) : (b) )

int AdtsReader::init(const char *filename) {
    fs.open(filename, std::ios::out | std::ios::binary);
    if (!fs.is_open()) {
        fprintf(stderr, "open %s failed\n", filename);
        return -1;
    }


    //最大ADTS头是9，没有差错校验就是7
    buffer = new uint8_t[MAX_DATA];
    fs.read(reinterpret_cast<char *>(buffer), MAX_DATA);
    blockBufferSize = fs.gcount();


    if (blockBufferSize < 7) {
        fprintf(stderr, "数据不完整\n");
        return -1;
    }

    if (memcmp(buffer, "ID3", 3) == 0) {
        fprintf(stderr, "不支持解析ID3\n");
        return -1;
    } else if (memcmp(buffer, "ADIF", 4) == 0) {
        fprintf(stderr, "不支持解析ADIF格式\n");
        return -1;
    }
    return 0;
}

int AdtsReader::adts_sequence(AdtsHeader &header, bool &stopFlag) {
    int ret;
    if (!buffer) {
        fprintf(stderr, "请初始化\n");
        return -1;
    }
    //while (true) {
    fillBuffer();
    if (blockBufferSize > 7) {
        ReadStream bs(buffer, MAX_DATA);
        if (bs.getMultiBit(12) != 0xFFF) {
            stopFlag = false;
            fprintf(stderr, "格式不对,不等于0xFFF\n");
            return -1;
            //break;
        }
        /*读取每一帧的ADTS头*/
        //AdtsHeader adtsHeader;
        ret = header.adts_fixed_header(bs);
        if (ret < 0) {
            stopFlag = false;
            fprintf(stderr, "解析adts fixed header失败\n");
            return -1;
        }
        header.adts_variable_header(bs);

        uint16_t frameLength = header.frame_length;

        /*如果这一帧的长度等于0或者大于filesize的话就退出，数据不对*/
        if (frameLength == 0 || frameLength > blockBufferSize) {
            stopFlag = false;
            fprintf(stderr, "AAC data 数据不对\n");
            return -1;
            //break;
        }
        header.data = &buffer[7];
        header.size = frameLength - 7;

        advanceBuffer(frameLength);
    } else {
        fillByteSize = 0;
        stopFlag = false;
        //break;
    }
    // }

    return 0;
}

int AdtsReader::fillBuffer() {
    if (fillByteSize > 0) {
        if (blockBufferSize) {
            //由src所指内存区域复制count个字节到dest所指内存区域。
            memmove(buffer, (buffer + fillByteSize), blockBufferSize);
        }

        if (isEof) {
            fs.read(reinterpret_cast<char *>(buffer + blockBufferSize), fillByteSize);
            uint32_t size = fs.gcount();

            /*如果读不满 fillByteSize 大小代表读到尾部了size是实际读取了多大*/
            if (size != fillByteSize) {
                isEof = false;
            }
            blockBufferSize += size;
        }
        fillByteSize = 0;
    }
    return 0;
}

int AdtsReader::advanceBuffer(uint16_t length) {
    if (length > 0 && blockBufferSize > 0) {
        uint32_t size = min(length, blockBufferSize);
        fillByteSize += size;
        blockBufferSize -= size;
    }
    return 0;
}

AdtsReader::~AdtsReader() {
    if (buffer) {
        delete[] buffer;
        buffer = nullptr;
    }
    fs.close();
}
