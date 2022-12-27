
#ifndef MUX_NALREADER_H
#define MUX_NALREADER_H

#include <cstdint>
#include <fstream>

class NaluReader {
private:
    static constexpr uint32_t MAX_BUFFER_SIZE{1024 * 1024};
    std::ifstream fs;
    uint8_t *bufferStart;
    uint8_t *bufferPosition;
    uint8_t *bufferEnd;

    uint32_t blockBufferSize{0};

private:
    static int getNextStartCode(uint8_t *bufPtr, const uint8_t *end, uint8_t *&pos1, uint8_t *&pos2, int &startCodeLen1,
                                int &startCodeLen2);

public:
    int init(const char *filename);

    int readNalUint(uint8_t *&data, uint32_t &size, int &startCodeLength, bool &isStopLoop);

    ~NaluReader();
};


#endif //MUX_NALREADER_H
