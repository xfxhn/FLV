#include <cstring>
#include "writeStream.h"

WriteStream::WriteStream(uint8_t *buf, uint32_t size) {

    memset(buf, 0, size);
    BufferStart = buf;
    currentPtr = buf;
    endPtr = buf + size - 1;
    bufferSize = size;
    position = 0;

}

uint8_t WriteStream::writeBit(uint8_t bit) {
    uint32_t ret = *currentPtr;
    --bitsLeft;
    uint8_t val = ret >> (bitsLeft);
    *currentPtr = (unsigned) (val | bit) << (bitsLeft);

    if (bitsLeft == 0) {
        currentPtr++;
        position++;
        bitsLeft = 8;
    }
    return 0;
}

uint64_t WriteStream::writeMultiBit(uint32_t n, uint32_t val) {
    uint8_t bitPos = n;
    for (int i = 0; i < n; ++i) {
        --bitPos;
        uint8_t bit = (val >> bitPos) & 1u;
        writeBit(bit);
    }
    return 0;
}
