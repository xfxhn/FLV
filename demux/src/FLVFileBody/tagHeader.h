
#ifndef FLV_TAGHEADER_H
#define FLV_TAGHEADER_H

#include <cstdint>

class ReadStream;

class TagHeader {
public:
    int parseData(ReadStream &rs);

public:
    uint8_t Filter{0};
    uint8_t TagType{0};
    uint32_t DataSize{0};
private:


    uint32_t Timestamp{0};
    uint8_t TimestampExtended{0};
    uint32_t realTimestamp{0};
    uint32_t StreamID{0};
};


#endif //FLV_TAGHEADER_H
