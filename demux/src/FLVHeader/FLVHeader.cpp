
#include "FLVHeader.h"
#include "readStream.h"
#include <cstring>
#include <cstdio>

//对于FLV版本1,DataOffset字段的值通常为9。这个字段的存在是为了在以后的版本中容纳更大的头文件
int FLVHeader::parse(ReadStream &rs) {

    rs.getString(Signature, 3);
    if (strcmp(Signature, "FLV") != 0) {
        fprintf(stderr, "this is not an FLV file.\n");
        return -1;
    }
    Version = rs.readMultiBit(8);

    //reserved
    rs.readMultiBit(5);
    //音频标签存在
    TypeFlagsAudio = rs.readBit();
    //reserved
    rs.readBit();
    //视频标签存在
    TypeFlagsVideo = rs.readBit();
    //此头的长度(以字节为单位)
    DataOffset = rs.readMultiBit(32);
    return 0;
}

uint32_t FLVHeader::getDataLength() const {
    return DataOffset;
}
