
#include "videoTag.h"

#include "AVCDecoderConfigurationRecord.h"
#include "writeStream.h"
#include "NALPicture.h"

static inline uint32_t bswap_32(uint32_t x) {
    x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
    return (x >> 16) | (x << 16);
}

int VideoTag::writeDesc(WriteStream &ws) const {
    ws.writeMultiBit(4, frameType);
    ws.writeMultiBit(4, codecID);
    ws.writeMultiBit(8, AVCPacketType);
    ws.writeMultiBit(24, compositionTime);
    return 5;
}

uint32_t VideoTag::writeData(const NALPicture *picture, std::ofstream &fs) {
    uint32_t size = 0;
    for (const auto &frame: picture->data) {
        const uint32_t length = bswap_32(frame.nalUintSize);
        fs.write(reinterpret_cast<const char *>(&length), 4);
        fs.write(reinterpret_cast<const char *>(frame.data), frame.nalUintSize);
        size += 4 + frame.nalUintSize;
    }
    return size;

}

int VideoTag::writeConfig(const NALPicture *picture, std::ofstream &fs) {

    const NALSliceHeader &sliceHeader = picture->sliceHeader;
    AVCDecoderConfigurationRecord AVCConfig;

    AVCConfig.configurationVersion = 1;
    AVCConfig.AVCProfileIndication = sliceHeader.sps.profile_idc;
    AVCConfig.AVCLevelIndication = sliceHeader.sps.level_idc;
    AVCConfig.lengthSizeMinusOne = 3;



    /*描述信息加sps和pps的大小*/
    int size = 11 + (int) picture->size;
    uint8_t *buffer = new uint8_t[size]; // NOLINT(modernize-use-auto)
    WriteStream ws(buffer, size);
    int ret = AVCConfig.write(ws, picture);
    if (ret < 0) {
        fprintf(stderr, "写入config失败\n");
        return ret;
    }


    fs.write(reinterpret_cast<const char *>(buffer), size);
    delete[] buffer;
    return size;
}
