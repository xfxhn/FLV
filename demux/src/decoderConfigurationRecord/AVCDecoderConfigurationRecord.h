#ifndef DEMUX_AVCDECODERCONFIGURATIONRECORD_H
#define DEMUX_AVCDECODERCONFIGURATIONRECORD_H

#include <cstdint>

class ReadStream;

class AVCDecoderConfigurationRecord {
public:

    uint8_t configurationVersion;
    uint8_t AVCProfileIndication;
    uint8_t profile_compatibility;
    uint8_t AVCLevelIndication;


    uint8_t numOfSequenceParameterSets;
    /* seq_parameter_set_id在0-31 的范围内，包括0和31 */
    uint16_t sequenceParameterSetLength[32]{0};
    uint8_t *sequenceParameterSetNALUnit[32]{nullptr};

    uint8_t numOfPictureParameterSets;
    /*pic_parameter_set_id的值应该0到255的范围内（包括0 和255）。*/
    uint16_t pictureParameterSetLength[256]{0};
    uint8_t *pictureParameterSetNALUnit[256]{nullptr};


    uint8_t chroma_format{1};
    uint8_t bit_depth_luma_minus8{0};
    uint8_t bit_depth_chroma_minus8{0};

    uint8_t numOfSequenceParameterSetExt{0};
    uint16_t sequenceParameterSetExtLength[256]{0};
    uint8_t *sequenceParameterSetExtNALUnit[256]{nullptr};

public:
    uint8_t lengthSizeMinusOne;

    int parseData(ReadStream &bs);

    ~AVCDecoderConfigurationRecord();
};

#endif //DEMUX_AVCDECODERCONFIGURATIONRECORD_H