
#ifndef FLV_AUDIOTAG_H
#define FLV_AUDIOTAG_H


#include <fstream>
#include <cstdint>

class ReadStream;

class AudioTag {
public:
    int parseData(std::ifstream &fs, uint32_t size, std::ofstream &audioOutFile);

private:
    /*
     * SoundData格式。定义了以下值:
     * 0 = Linear PCM, platform endian
     * 1 = ADPCM
     * 2 = MP3
     * 3 = Linear PCM, little endian
     * 10 = AAC
     * */
    uint8_t SoundFormat;
    /*
        采样率 对于AAC来说，该值总是3
        0 = 5.5 kHz
        1 = 11 kHz
        2 = 22 kHz
        3 = 44 kHz
     */
    uint8_t SoundRate;
    /*
每个采样点数据的大小， 这个字段只左右于未压缩格式 ,PCM 就是未压缩格式。而AAC这类就是已经压缩过的格式了。这个字段总是1
0 = 8-bit samples
1 = 16-bit samples*/
    uint8_t SoundSize;
    /*
     单声道或立体声 对于AAC的话总是1
     0 = Mono sound
     1 = Stereo sound
     */
    uint8_t SoundType;
    /*
是AAC编码的才有这个值
0 = AAC sequence header
1 = AAC raw
     */
    uint8_t AACPacketType;


    static int analysisRawAACData(ReadStream &rs, std::ofstream &file);
};


#endif //FLV_AUDIOTAG_H
