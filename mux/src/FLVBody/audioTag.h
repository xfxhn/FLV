

#ifndef MUX_AUDIOTAG_H
#define MUX_AUDIOTAG_H

#include <cstdint>
#include <fstream>

class WriteStream;

class AdtsHeader;

class AudioTag {
public:
    /*
     * SoundData格式。定义了以下值:
     * 0 = Linear PCM, platform endian
     * 1 = ADPCM
     * 2 = MP3
     * 3 = Linear PCM, little endian
     * 10 = AAC
     * */
    uint8_t SoundFormat{10};
    /*
        采样率 对于AAC来说，该值总是3
        0 = 5.5 kHz
        1 = 11 kHz
        2 = 22 kHz
        3 = 44 kHz
     */
    uint8_t SoundRate{3};
    /*
每个采样点数据的大小， 这个字段只左右于未压缩格式 ,PCM 就是未压缩格式。而AAC这类就是已经压缩过的格式了。这个字段总是1
0 = 8-bit samples
1 = 16-bit samples*/
    uint8_t SoundSize{1};
    /*
     单声道或立体声 对于AAC的话总是1
     0 = Mono sound
     1 = Stereo sound
     */
    uint8_t SoundType{1};
    /*
是AAC编码的才有这个值
0 = AAC sequence header
1 = AAC raw
     */
    uint8_t AACPacketType{0};
public:
    int writeDesc(WriteStream &ws) const;

    static int writeConfig(const AdtsHeader &info, std::ofstream &fs);

    static uint32_t writeData(const AdtsHeader &info, std::ofstream &fs);
};


#endif //MUX_AUDIOTAG_H
