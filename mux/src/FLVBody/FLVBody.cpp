
#include "FLVBody.h"

#include "tagHeader.h"
#include "videoTag.h"
#include "audioTag.h"
#include "scriptTag.h"
#include "writeStream.h"


#include "adtsHeader.h"
#include "NALPicture.h"


uint32_t FLVBody::writeScript(std::ofstream &fs) {

    uint32_t size = 0;
    TagHeader header;
    header.Filter = 0;
    header.TagType = 18;
    header.DataSize = 413;
    header.Timestamp = 0;
    header.TimestampExtended = 0;

    uint8_t tagHeaderBuffer[11];
    WriteStream ws(tagHeaderBuffer, 11);
    size += header.write(ws);
    fs.write(reinterpret_cast<const char *>(tagHeaderBuffer), 11);


    //ScriptTag tag;
    ScriptTag::write(fs);
    return size + 413;
}

uint32_t FLVBody::writeVideo(const NALPicture *picture, std::ofstream &fs) {

    uint32_t size = 0;
    TagHeader header;
    header.Filter = 0;
    header.TagType = 9;
    /*tag body不包含这个header 的描述信息大小*/
    header.DataSize = 5 + picture->size + picture->data.size() * 4;
    header.Timestamp = picture->videoDts & 0xFFFFFF;
    header.TimestampExtended = picture->videoDts >> 24 & 0xFF;

    uint8_t tagHeaderBuffer[11];
    WriteStream ws1(tagHeaderBuffer, 11);
    size += header.write(ws1);
    fs.write(reinterpret_cast<const char *>(tagHeaderBuffer), 11);


    VideoTag tag;
    tag.frameType = picture->sliceHeader.nalu.IdrPicFlag ? 1 : 2;
    tag.codecID = 7;
    tag.AVCPacketType = 1;
    tag.compositionTime = picture->videoPts - picture->videoDts;

    uint8_t tagBodyBuffer[5];
    WriteStream ws2(tagBodyBuffer, 5);
    size += tag.writeDesc(ws2);
    fs.write(reinterpret_cast<const char *>(tagBodyBuffer), 5);

    /*输出实际数据*/
    size += VideoTag::writeData(picture, fs);


    return size;
}

uint32_t FLVBody::writeAudio(const AdtsHeader &info, std::ofstream &fs) {

    uint32_t size = 0;
    TagHeader header;
    header.Filter = 0;
    header.TagType = 8;
    /*tag body不包含这个header 的描述信息大小*/
    header.DataSize = 2 + info.size;
    header.Timestamp = info.dts & 0xFFFFFF;
    header.TimestampExtended = info.dts >> 24 & 0xFF;

    uint8_t tagHeaderBuffer[11];
    WriteStream ws1(tagHeaderBuffer, 11);
    size += header.write(ws1);
    fs.write(reinterpret_cast<const char *>(tagHeaderBuffer), 11);


    AudioTag tag;
    tag.AACPacketType = 1;

    uint8_t tagBodyBuffer[2];
    WriteStream ws2(tagBodyBuffer, 2);
    size += tag.writeDesc(ws2);
    fs.write(reinterpret_cast<const char *>(tagBodyBuffer), 2);


    /*输出实际数据*/
    size += AudioTag::writeData(info, fs);
    return size;
}

uint32_t FLVBody::writeVideoConfig(const NALPicture *picture, std::ofstream &fs) {


    uint32_t size = 0;
    TagHeader header;
    header.Filter = 0;
    header.TagType = 9;
    /*tag body不包含这个header 的描述信息大小
     * 5是tag body desc大小
     * 11是avc config大小
     * picture->size是sps和pps大小
     * */
    header.DataSize = 5 + 11 + picture->size;
    header.Timestamp = 0;
    header.TimestampExtended = 0;

    uint8_t tagHeaderBuffer[11];
    WriteStream ws1(tagHeaderBuffer, 11);
    size += header.write(ws1);
    fs.write(reinterpret_cast<const char *>(tagHeaderBuffer), 11);


    VideoTag tag;
    tag.frameType = 1;
    tag.codecID = 7;
    tag.AVCPacketType = 0;
    tag.compositionTime = 0;

    uint8_t tagBodyBuffer[5];
    WriteStream ws2(tagBodyBuffer, 5);
    size += tag.writeDesc(ws2);
    fs.write(reinterpret_cast<const char *>(tagBodyBuffer), 5);

    /*输出config*/
    int ret = VideoTag::writeConfig(picture, fs);
    if (ret < 0) {
        fprintf(stderr, "写入config失败");
        exit(-1);
    }
    size += ret;
    return size;
}

uint32_t FLVBody::writeAudioConfig(const AdtsHeader &info, std::ofstream &fs) {

    uint32_t size = 0;
    TagHeader header;
    header.Filter = 0;
    header.TagType = 8;

    header.DataSize = 2 + 2;
    header.Timestamp = 0;
    header.TimestampExtended = 0;

    uint8_t tagHeaderBuffer[11];
    WriteStream ws1(tagHeaderBuffer, 11);
    size += header.write(ws1);
    fs.write(reinterpret_cast<const char *>(tagHeaderBuffer), 11);


    AudioTag tag;
    tag.AACPacketType = 0;
    uint8_t tagBodyBuffer[2];
    WriteStream ws2(tagBodyBuffer, 2);
    size += tag.writeDesc(ws2);
    fs.write(reinterpret_cast<const char *>(tagBodyBuffer), 2);



    /*输出config*/
    int ret = AudioTag::writeConfig(info, fs);
    if (ret < 0) {
        fprintf(stderr, "写入config失败");
        exit(-1);
    }
    size += ret;


    return size;
}

uint32_t FLVBody::flushVideo(std::ofstream &fs) {
    int32_t size = 0;
    TagHeader header;
    header.Filter = 0;
    header.TagType = 9;
    header.DataSize = 5;
    header.Timestamp = 0;
    header.TimestampExtended = 0;

    uint8_t tagHeaderBuffer[11];
    WriteStream ws1(tagHeaderBuffer, 11);
    size += header.write(ws1);
    fs.write(reinterpret_cast<const char *>(tagHeaderBuffer), 11);


    VideoTag tag;
    tag.frameType = 1;
    tag.codecID = 7;
    tag.AVCPacketType = 2;
    tag.compositionTime = 0;

    uint8_t tagBodyBuffer[5];
    WriteStream ws2(tagBodyBuffer, 5);
    size += tag.writeDesc(ws2);
    fs.write(reinterpret_cast<const char *>(tagBodyBuffer), 5);
    return size;
}

uint32_t FLVBody::flushAudio(std::ofstream &fs) {
    int32_t size = 0;
    TagHeader header;
    header.Filter = 0;
    header.TagType = 8;
    header.DataSize = 2;
    header.Timestamp = 0;
    header.TimestampExtended = 0;

    uint8_t tagHeaderBuffer[11];
    WriteStream ws1(tagHeaderBuffer, 11);
    size += header.write(ws1);
    fs.write(reinterpret_cast<const char *>(tagHeaderBuffer), 11);


    AudioTag tag;
    /*tag.AVCPacketType = 2;*/
    tag.AACPacketType = 2;
    uint8_t tagBodyBuffer[2];
    WriteStream ws2(tagBodyBuffer, 2);
    size += tag.writeDesc(ws2);
    fs.write(reinterpret_cast<const char *>(tagBodyBuffer), 2);
    return size;
}




