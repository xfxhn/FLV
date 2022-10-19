#include "videoTag.h"
#include "readStream.h"
#include "AVCDecoderConfigurationRecord.h"


static AVCDecoderConfigurationRecord AVCConfig;


int VideoTag::parseData(std::ifstream &fs, uint32_t size, std::ofstream &videoOutFile) {

    uint8_t *buf = new uint8_t[size]; // NOLINT(modernize-use-auto)
    fs.read(reinterpret_cast<char *>(buf), size);
    ReadStream rs(buf, size);

    frameType = rs.readMultiBit(4);

    codecID = rs.readMultiBit(4);

    /*只有是avc编码的时候才有这两个值*/
    if (codecID == 7) {
        /*
            0：AVC sequence header，也就是sps和pps
            1：AVC NALU
            2：AVC end of sequence
         */
        AVCPacketType = rs.readMultiBit(8);
        /*
         * 表示pts相对于该帧dts的差值
         * 当B帧存在时，视频帧的pts和dts可能不同，dts在flv中是指tag header中的timestamp。
         * 所以视频帧的pts = CompositionTime + timestamp
         * 如果B帧不存在，则CTS固定为0，也就是CompositionTime这个栏位0,
         * AVCPacketType == 1这个值才有效，否则=0
         * */
        compositionTime = rs.readMultiBit(24);

        if (AVCPacketType == 0) {
            AVCConfig.parseData(rs);
        } else if (AVCPacketType == 1) {
            analysisNalUint(rs, videoOutFile);
        }
    } else {
        delete[] buf;
        fprintf(stderr, "不支持除了AVC以为的编码标准\n");
        return -1;
    }

    delete[] buf;
    return 0;
}


int VideoTag::analysisNalUint(ReadStream &rs, std::ofstream &file) {
    uint8_t startCode[4] = {0, 0, 0, 1};


    while (true) {
        if (rs.position >= rs.size) {
            break;
        }
        uint32_t nalUintSize = rs.readMultiBit((AVCConfig.lengthSizeMinusOne + 1) * 8);
        {
            uint8_t nalUnit = rs.getMultiBit(8) & 31;
            int aaaaaa = 1;
        }
        file.write(reinterpret_cast<const char *>(startCode), 4);
        file.write(reinterpret_cast<const char *>(rs.currentPtr), nalUintSize);
        rs.setBytePtr(nalUintSize);

    }

    return 0;
}
