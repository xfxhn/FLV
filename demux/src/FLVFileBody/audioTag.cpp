
#include "audioTag.h"
#include "readStream.h"
#include "audioSpecificConfig.h"
#include "adtsHeader.h"
#include "writeStream.h"

static AudioSpecificConfig ASCConfig;

int AudioTag::parseData(std::ifstream &fs, uint32_t size, std::ofstream &audioOutFile) {
    uint8_t *buf = new uint8_t[size]; // NOLINT(modernize-use-auto)
    fs.read(reinterpret_cast<char *>(buf), size);
    ReadStream rs(buf, size);


    SoundFormat = rs.readMultiBit(4);
    SoundRate = rs.readMultiBit(2);
    SoundSize = rs.readBit();
    SoundType = rs.readBit();

    if (SoundFormat == 10) {
        AACPacketType = rs.readMultiBit(8);

        if (AACPacketType == 0) {
            ASCConfig.parseData(rs);
        } else if (AACPacketType == 1) {
            analysisRawAACData(rs, audioOutFile);
        }
    } else {
        delete[] buf;
        fprintf(stderr, "不支持除了AAC以为的编码标准\n");
        return -1;
    }


    delete[] buf;
    return 0;
}

int AudioTag::analysisRawAACData(ReadStream &rs, std::ofstream &file) {
    uint32_t dataSize = rs.size - rs.position;

    uint8_t adts_header_buf[7]{0};
    WriteStream ws(adts_header_buf, 7);

    AdtsHeader header(
            dataSize, ASCConfig.audioObjectType - 1,
            ASCConfig.sampleRate, ASCConfig.channelConfiguration);
    header.adts_fixed_header(ws);
    header.adts_variable_header(ws);


    /*std::ofstream file("resource/test.aac", std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        fprintf(stderr, "open file failed\n");
        return -1;
    }*/
    file.write(reinterpret_cast<const char *>(adts_header_buf), 7);
    file.write(reinterpret_cast<const char *>(rs.currentPtr), dataSize);
    rs.setBytePtr(dataSize);
    //file.close();

    /*while (true) {
        if (rs.position >= rs.size) {
            int a = 1;
            break;
        }


    }*/

    return 0;
}
