
#ifndef MUX_TAGHEADER_H
#define MUX_TAGHEADER_H

#include <cstdint>

class WriteStream;

class TagHeader {
public:
    int write(WriteStream &ws) const;

public:
    uint8_t Filter{0};
    uint8_t TagType{0};
    uint32_t DataSize{0};


    uint32_t Timestamp{0};
    uint8_t TimestampExtended{0};
private:
    uint32_t StreamID{0};
};


#endif //MUX_TAGHEADER_H
