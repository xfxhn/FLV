#ifndef MUX_VIDEOTAG_H
#define MUX_VIDEOTAG_H

#include <fstream>
#include <cstdint>

class WriteStream;

class NALPicture;

class VideoTag {
public:
    uint8_t frameType{0};
    /*编码标准*/
    uint8_t codecID{7};
    uint8_t AVCPacketType{0};
    uint32_t compositionTime{0};
public:
    int writeDesc(WriteStream &ws) const;

    static int writeConfig(const NALPicture *picture, std::ofstream &fs);

    static uint32_t writeData(const NALPicture *picture, std::ofstream &fs) ;


};


#endif //MUX_VIDEOTAG_H
