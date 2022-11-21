
#include "audioTag.h"

#include "writeStream.h"
#include "audioSpecificConfig.h"
#include "adtsHeader.h"

int AudioTag::writeDesc(WriteStream &ws) const {
    ws.writeMultiBit(4, SoundFormat);
    ws.writeMultiBit(2, SoundRate);
    ws.writeMultiBit(1, SoundSize);
    ws.writeMultiBit(1, SoundType);
    ws.writeMultiBit(8, AACPacketType);
    return 2;
}

int AudioTag::writeConfig(const AdtsHeader &info, std::ofstream &fs) {
    AudioSpecificConfig AACConfig;

    if (info.channel_configuration == 0) {
        fprintf(stderr, "声道数=0，写入config失败\n");
        return -1;
    }
    AACConfig.audioObjectType = info.ID == 1 ? info.profile + 1 : info.profile;
    AACConfig.samplingFrequencyIndex = info.sampling_frequency_index;
    AACConfig.channelConfiguration = info.channel_configuration;
    AACConfig.frameLengthFlag = false;
    AACConfig.extensionFlag = false;





    /*aacConfig 大小*/
    constexpr int size = 2;
    uint8_t buffer[size];
    WriteStream ws(buffer, size);
    AACConfig.write(ws);

    fs.write(reinterpret_cast<const char *>(buffer), size);
    return size;
}

uint32_t AudioTag::writeData(const AdtsHeader &info, std::ofstream &fs) {
    fs.write(reinterpret_cast<const char *>(info.data), info.size);
    return info.size;
}
