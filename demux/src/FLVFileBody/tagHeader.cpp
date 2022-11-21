
#include "tagHeader.h"

#include <cstdio>

#include "readStream.h"

int TagHeader::parseData(ReadStream &rs) {

    /*Reserved*/
    rs.readMultiBit(2);
    /* 报文是否被过滤。
     * 0 =不需要预处理。
     * 1 =在呈现数据包之前，需要对其进行预处理(例如解密)。
     * 在未加密的文件中为0，在加密的标签中为1。
     * 具体参考附录F FLV加密
     * */
    Filter = rs.readBit();
    /*
     * 此tag中的内容类型
     * 8 = audio
     * 9 = video
     * 18 = script data
     * */
    TagType = rs.readMultiBit(5);

    /*表明当前tag中存储的音频或视频文件的大小，包括文件的头*/
    /*消息的长度。从StreamID到标签结尾的字节数(等于标签的长度- 11)*/
    DataSize = rs.readMultiBit(24);
    /*此tag中的数据的时间(以毫秒为单位)。这个值相对于FLV文件中的第一个标记时间戳总是为0*/
    Timestamp = rs.readMultiBit(24);
    /*扩展时间戳 如果Timestamp⼤于0xFFFFFF，将会使⽤这个字节。这个字节是时戳的⾼8位，上⾯的三个字节是低24位。*/
    TimestampExtended = rs.readMultiBit(8);

    realTimestamp = TimestampExtended << 24 | Timestamp;
    /*if (Timestamp > 0xFFFFFF) {
        realTimestamp = TimestampExtended << 24 | Timestamp;
    } else {
        realTimestamp = Timestamp;
    }*/
    /*if (TagType == 9) {
        printf("realTimestamp=%d\n", realTimestamp);

    }*/


    StreamID = rs.readMultiBit(24);
    return 0;
}
