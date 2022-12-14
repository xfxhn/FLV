#include "readStream.h"


ReadStream::ReadStream(uint8_t *buf, uint32_t _size) {
    start = buf;
    currentPtr = buf;
    endPtr = buf + _size - 1;
    size = _size;
    position = 0;
}

uint8_t ReadStream::readBit() {
    --bitsLeft;

    uint8_t result = ((unsigned) (*currentPtr) >> bitsLeft) & 1u;

    if (bitsLeft == 0) {
        currentPtr++;
        position++;
        bitsLeft = 8;
    }
    return result;
}

uint64_t ReadStream::readMultiBit(uint32_t n) {
    uint64_t result = 0;
    for (uint32_t i = 0; i < n; ++i) {
        //把前n位移到后面n位来
        //readBit()在算数表达式小于int类型，会被扩展为整型
        result = result | ((unsigned) readBit() << (n - i - 1u));
    }
    return result;
}

uint64_t ReadStream::getMultiBit(uint32_t n) {
    uint64_t ret = 0;

    uint8_t *tempPtr = currentPtr;
    uint8_t tempBitsLeft = bitsLeft;
    uint32_t tempPosition = position;

    ret = readMultiBit(n);

    currentPtr = tempPtr;
    bitsLeft = tempBitsLeft;
    position = tempPosition;
    return ret;
}

int ReadStream::getString(char str[], uint32_t n) {
    for (int i = 0; i < n; ++i) {
        char c = (char) readMultiBit(8);
        str[i] = c;
    }
    return 0;
}

int ReadStream::setBytePtr(uint32_t n) {
    currentPtr += n;
    position += n;
    bitsLeft = 8;
    return 0;
}

int ReadStream::byteAlignment() {
    while (bitsLeft != 8) {
        readBit();
    }
    return 0;
}

uint32_t ReadStream::bitsToDecode() const {
    return (size - position - 1u) * 8 + bitsLeft;
}
