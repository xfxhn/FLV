
#ifndef FLV_AUDIOSPECIFICCONFIG_H
#define FLV_AUDIOSPECIFICCONFIG_H

#include <cstdint>
#include <fstream>

class ReadStream;

class WriteStream;

class AdtsHeader;

class AudioSpecificConfig {
public:
    int parseData(ReadStream &rs);

    int write(WriteStream &ws) const;


public:
    uint8_t audioObjectType;
    uint32_t sampleRate;
    uint8_t channelConfiguration;

    uint8_t samplingFrequencyIndex;
    uint32_t samplingFrequency;


    int sbrPresentFlag{-1};
    int psPresentFlag{-1};
    /*一个5bit字段，表示扩展音频对象类型。该对象类型对应于一个扩展工具，该工具用于增强底层的audioObjectType*/
    uint8_t extensionAudioObjectType;
    uint8_t extensionSamplingFrequencyIndex;
    uint32_t extensionSamplingFrequency;
    uint8_t extensionChannelConfiguration;


    bool frameLengthFlag{false};
    bool dependsOnCoreCoder{false};
    bool extensionFlag{false};


    uint16_t syncExtensionType;
private:


    static uint8_t GetAudioObjectType(ReadStream &rs);

    int GASpecificConfig(ReadStream &rs);

    static int program_config_element(ReadStream &rs);


};


#endif //FLV_AUDIOSPECIFICCONFIG_H
