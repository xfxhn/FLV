
#ifndef MUX_ADTSREADER_H
#define MUX_ADTSREADER_H

#include <fstream>

class AdtsHeader;

class AdtsReader {
private:
    std::ifstream fs;
    uint8_t *buffer{nullptr};


    uint32_t blockBufferSize{0};
    /*已经读取了多少字节*/
    uint32_t fillByteSize{0};
    bool isEof{true};
public:
    int init(const char *filename);

    int adts_sequence(AdtsHeader &header, bool &stopFlag);

    ~AdtsReader();

private:
    int fillBuffer();

    int advanceBuffer(uint16_t frameLength);

};


#endif //MUX_ADTSREADER_H
