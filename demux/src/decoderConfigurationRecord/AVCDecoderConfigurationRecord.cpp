#include <cstring>
#include "AVCDecoderConfigurationRecord.h"
#include "readStream.h"


int AVCDecoderConfigurationRecord::parseData(ReadStream &bs) {
    configurationVersion = bs.readMultiBit(8);
    /* 编码等级 */
    AVCProfileIndication = bs.readMultiBit(8);
    /* 应该是 level_IDC */
    /* 支持的分辨率、最大引用数、帧速率等。见H.264/AVC附件A。 */
    profile_compatibility = bs.readMultiBit(8);
    AVCLevelIndication = bs.readMultiBit(8);
    /* 保留 */
    bs.readMultiBit(6);
    /*表示用多少字节存储每个nalu的长度，计算方法是 1+lengthSizeMinusOne */
    lengthSizeMinusOne = bs.readMultiBit(2);
    /* 保留 */
    bs.readMultiBit(3);

    /* numOfSequenceParameterSets表示用于解码AVC基本流的初始SPSs集的数量。(sps的个数) */
    numOfSequenceParameterSets = bs.readMultiBit(5);
    for (int i = 0; i < numOfSequenceParameterSets; ++i) {
        /* sps多少字节 */
        sequenceParameterSetLength[i] = bs.readMultiBit(16);
        sequenceParameterSetNALUnit[i] = new uint8_t[sequenceParameterSetLength[i]]();
        memcpy(sequenceParameterSetNALUnit[i], bs.currentPtr, sequenceParameterSetLength[i]);
        bs.readMultiBit(sequenceParameterSetLength[i] * 8);
    }

    numOfPictureParameterSets = bs.readMultiBit(8);
    for (int j = 0; j < numOfPictureParameterSets; ++j) {
        /* pps多少字节 */
        pictureParameterSetLength[j] = bs.readMultiBit(16);
        pictureParameterSetNALUnit[j] = new uint8_t[pictureParameterSetLength[j]]();
        memcpy(pictureParameterSetNALUnit[j], bs.currentPtr, pictureParameterSetLength[j]);
        bs.readMultiBit(pictureParameterSetLength[j] * 8);
    }

//    uint8_t profile_idc = AVCProfileIndication;
//    /*
//        66	Baseline
//        77	Main
//        88	Extended
//        100	High(FRExt)
//        110	High 10 (FRExt)
//        122	High 4:2 : 2 (FRExt)
//        144	High 4 : 4 : 4 (FRExt)
//    */
//    if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 144) {
//        bs.readMultiBit(6);
//
//        /* chroma_format_idc = 0	单色
//        chroma_format_idc = 1	YUV 4 : 2 : 0
//        chroma_format_idc = 2	YUV 4 : 2 : 2
//        chroma_format_idc = 3	YUV 4 : 4 : 4
//        当chroma_format_idc不存在时，它将被推断为等于1(4:2:0色度格式)。*/
//        chroma_format = bs.readMultiBit(2);
//        bs.readMultiBit(5);
//        //亮度用几bit表示，这个值是个偏移（bit位深度）
//        /* (BitDepth = 8 + bit_depth_luma_minus8) */
//        bit_depth_luma_minus8 = bs.readMultiBit(3);
//        bs.readMultiBit(5);
//        /* 色度位深度的偏移 */
//        bit_depth_chroma_minus8 = bs.readMultiBit(3);
//        /* numOfSequenceParameterSetExt表示用于AVC基本流解码的序列参数集扩展的数量 */
//        numOfSequenceParameterSetExt = bs.readMultiBit(8);
//        for (int i = 0; i < numOfSequenceParameterSetExt; ++i) {
//            sequenceParameterSetExtLength[i] = bs.readMultiBit(16);
//            sequenceParameterSetExtNALUnit[i] = new uint8_t[sequenceParameterSetExtLength[i]]();
//            memcpy(sequenceParameterSetExtNALUnit[i], bs.currentPtr, sequenceParameterSetExtLength[i]);
//            bs.readMultiBit(sequenceParameterSetExtLength[i] * 8);
//        }
//    }
    return 0;
}

AVCDecoderConfigurationRecord::~AVCDecoderConfigurationRecord() {

    for (int i = 0; i < numOfSequenceParameterSets; ++i) {
        if (sequenceParameterSetNALUnit[i]) {
            delete[] sequenceParameterSetNALUnit[i];
            sequenceParameterSetNALUnit[i] = nullptr;
        }
    }

    for (int j = 0; j < numOfPictureParameterSets; ++j) {
        if (pictureParameterSetNALUnit[j]) {
            delete[] pictureParameterSetNALUnit[j];
            pictureParameterSetNALUnit[j] = nullptr;
        }
    }
    for (int j = 0; j < numOfSequenceParameterSetExt; ++j) {
        if (sequenceParameterSetExtNALUnit[j]) {
            delete[] sequenceParameterSetExtNALUnit[j];
            sequenceParameterSetExtNALUnit[j] = nullptr;
        }
    }
}