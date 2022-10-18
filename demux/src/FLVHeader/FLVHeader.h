#ifndef FLV_FLVHEADER_H
#define FLV_FLVHEADER_H

#include <cstdint>

class ReadStream;

class FLVHeader {
public:

    int parse(ReadStream &rs);

    uint32_t getDataLength() const;

private:
    char Signature[4]{0};
    uint8_t Version{0};
    //uint8_t TypeFlagsReserved{0};
    uint8_t TypeFlagsAudio{0};
    //uint8_t TypeFlagsReserved{0};
    uint8_t TypeFlagsVideo{0};
    uint32_t DataOffset{0};
};


#endif //FLV_FLVHEADER_H
