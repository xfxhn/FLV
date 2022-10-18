#ifndef FLV_DEMUX_H
#define FLV_DEMUX_H

#include <fstream>

class Demux {

public:
    Demux(const char *filename);


    int init();

    int demux();

private:
    // 每次读取的字节数，1024kb 1m
    static constexpr int READ_MIN_BYTE_SIZE{1024 * 1024};
    std::ifstream fs;
    const char *filename;

    uint8_t *buffer{nullptr};

    /*每次循环读取的数目*/
    uint32_t readFileSize{0};
    /*每次循环需要填充的数目*/
    uint32_t fillByteSize{0};
    bool isEof{true};
private:
    int fillBuffer();

    int advanceBuffer(uint32_t length);
};


#endif //FLV_DEMUX_H
