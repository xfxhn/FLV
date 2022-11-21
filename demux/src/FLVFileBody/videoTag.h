
#ifndef FLV_VIDEOTAG_H
#define FLV_VIDEOTAG_H

#include <fstream>
#include <cstdint>

class ReadStream;

class VideoTag {
private:
    /*视频帧的类型*/
    /*
     1 =关键帧idr帧(对于AVC，一个可寻帧)
     2 =普通i帧或者p b帧

     */
    uint8_t frameType;
    /*编码标准*/
    uint8_t codecID;
    uint8_t AVCPacketType;
    uint32_t compositionTime;

private:

    static int analysisNalUint(ReadStream &rs, std::ofstream &file);

public:
    int parseData(std::ifstream &fs, uint32_t size, std::ofstream &videoOutFile, uint32_t realTimestamp);
};


#endif //FLV_VIDEOTAG_H
