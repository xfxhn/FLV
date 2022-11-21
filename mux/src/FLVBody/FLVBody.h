
#ifndef MUX_FLVBODY_H
#define MUX_FLVBODY_H

#include <fstream>

class WriteStream;

class NALPicture;

class AdtsHeader;

class FLVBody {
public:
    static uint32_t writeScript(std::ofstream &fs);

    static uint32_t writeVideo(const NALPicture *picture, std::ofstream &fs);

    static uint32_t writeAudio(const AdtsHeader &info, std::ofstream &fs);

    static uint32_t writeVideoConfig(const NALPicture *picture, std::ofstream &fs);

    static uint32_t writeAudioConfig(const AdtsHeader &info, std::ofstream &fs);

    static uint32_t flushVideo(std::ofstream &fs);
    static uint32_t flushAudio(std::ofstream &fs);

};


#endif //MUX_FLVBODY_H
