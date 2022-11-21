

#include "tagHeader.h"
#include "writeStream.h"

int TagHeader::write(WriteStream &ws) const {

    ws.writeMultiBit(2, 0);
    ws.writeMultiBit(1, Filter);
    ws.writeMultiBit(5, TagType);
    /*除了这个header，实际存储多少数据的大小*/
    ws.writeMultiBit(24, DataSize);
    ws.writeMultiBit(24, Timestamp);
    ws.writeMultiBit(8, TimestampExtended);
    ws.writeMultiBit(24, StreamID);

    return 11;
}
