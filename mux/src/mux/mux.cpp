
#include "mux.h"

#include "writeStream.h"
#include "readStream.h"
#include "FLVHeader.h"
#include "FLVBody.h"

#include "NALReader.h"
#include "NALHeader.h"
#include "adtsReader.h"
#include "adtsHeader.h"
#include "NALSliceHeader.h"
#include "NALPicture.h"


enum NalUintType {
    H264_NAL_UNSPECIFIED = 0,
    H264_NAL_SLICE = 1,
    H264_NAL_IDR_SLICE = 5,
    H264_NAL_SEI = 6,
    H264_NAL_SPS = 7,
    H264_NAL_PPS = 8
};


inline static uint32_t bswap_32(uint32_t x) {
    x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
    return (x >> 16) | (x << 16);
}

/*   25   {1,25}  {1,1000}     */
int64_t Mux::av_rescale_q(int64_t a, AVRational bq, AVRational cq) {
    //(1 / 25) / (1 / 1000);
    int64_t b = bq.num * cq.den;
    int64_t c = cq.num * bq.den;
    return a * b / c;  //25 * (1000 / 25)  把1000分成25份，然后当前占1000的多少
}

int Mux::initAudio(const char *filename) {
    int ret;

    uint32_t audioFileSize = 0;
    uint32_t decodeFrameNumber = 0;
    AdtsReader reader;
    ret = reader.init(filename);
    if (ret < 0) {
        fprintf(stderr, "init Audio failed\n");
        return ret;
    }

    AdtsHeader header;
    bool stopFlag = true;

    int64_t tick = -2048;
    while (stopFlag) {
        ret = reader.adts_sequence(header, stopFlag);
        if (ret < 0) {
            fprintf(stderr, "解析adts_sequence 失败\n");
            return ret;
        }
        ++decodeFrameNumber;
        audioFileSize += header.frame_length;
        header.dts = av_rescale_q(tick, {1, static_cast<int>(header.sample_rate)}, {1, 1000});
        header.pts = header.dts;
        tick += 1024;
        if (aacFlag) {
            sequenceOfDecodingOutputAudioFrame(header);
            info.audiosamplerate = header.sample_rate;
            info.audiosamplesize = 16;
            info.stereo = header.channel_configuration == 2;
            aacFlag = false;
        }
        sequenceOfDecodingOutputAudioFrame(header);
    }

    info.audioDuration = decodeFrameNumber / (info.audiosamplerate / 1024);
    info.audiodatarate = (double) audioFileSize / 1024 * 8 / info.audioDuration;
    return ret;
}


int Mux::initVideo(const char *filename) {

    int ret;
    uint32_t videoFileSize = 0;


    NaluReader nalUint;
    ret = nalUint.init(filename);
    if (ret < 0) {
        fprintf(stderr, "init video failed\n");
        return ret;
    }


    uint8_t *data = nullptr;
    uint32_t size = 0;
    int startCodeLength = 0;
    bool isStopLoop = true;

    NALHeader nalUnitHeader;
    NALDecodedPictureBuffer gop;
    NALPicture *picture = gop.dpb[0];
    uint32_t decodeFrameNumber = 0;
    uint32_t decodeIdrFrameNumber = 0;

    while (isStopLoop) {
        nalUint.readNalUint(data, size, startCodeLength, isStopLoop);

        videoFileSize += size + startCodeLength;
        /*读取nalu header*/
        nalUnitHeader.nal_unit(data[0]);
        //NALHeader::ebsp_to_rbsp(data, size);


        if (nalUnitHeader.nal_unit_type == H264_NAL_SPS) {
            uint8_t *buf = new uint8_t[size];
            memcpy(buf, data, size);

            picture->size += size;
            picture->data.push_back({size, buf, 1});


            NALHeader::ebsp_to_rbsp(data, size);
            ReadStream rs(data, size);

            NALSeqParameterSet sps;
            sps.seq_parameter_set_data(rs);
            spsList[sps.seq_parameter_set_id] = sps;


        } else if (nalUnitHeader.nal_unit_type == H264_NAL_PPS) {
            uint8_t *buf = new uint8_t[size];
            memcpy(buf, data, size);

            picture->size += size;
            picture->data.push_back({size, buf, 2});

            NALHeader::ebsp_to_rbsp(data, size);
            ReadStream rs(data, size);

            NALPictureParameterSet pps;
            pps.pic_parameter_set_rbsp(rs, spsList);
            ppsList[pps.pic_parameter_set_id] = pps;


        } else if (nalUnitHeader.nal_unit_type == H264_NAL_SLICE || nalUnitHeader.nal_unit_type == H264_NAL_IDR_SLICE) {

            const uint32_t tempSize = size;
            uint8_t *buf = new uint8_t[tempSize]; // NOLINT(modernize-use-auto)
            memcpy(buf, data, tempSize);

            NALHeader::ebsp_to_rbsp(data, size);
            ReadStream rs(data, size);
            NALSliceHeader sliceHeader;
            sliceHeader.slice_header(rs, nalUnitHeader, spsList, ppsList);

            /*先处理上一帧，如果tag有数据并且first_mb_in_slice=0表示有上一针*/
            if (sliceHeader.first_mb_in_slice == 0 && picture->useFlag) {

                if (picture->pictureOrderCount == 0) {
                    decodeIdrFrameNumber = decodeFrameNumber * 2 + picture->pictureOrderCount;
                }
                picture->videoPts =
                        av_rescale_q((decodeIdrFrameNumber + picture->pictureOrderCount) / 2, sliceHeader.sps.timeBase,
                                     {1, 1000});
                picture->videoDts = av_rescale_q(decodeFrameNumber++, sliceHeader.sps.timeBase, {1, 1000}) - 80;

                /*
                 * 参考图片标记过程
                 * 当前图片的所有SLICE都被解码。
                 * 参考8.2.5.1第一条规则
                 * */
                NALPicture *unoccupiedPicture;
                ret = gop.decoding_finish(picture, unoccupiedPicture);
                if (ret < 0) {
                    fprintf(stderr, "图像解析失败\n");
                    return ret;
                }
                /*按照显示顺序输出*/
                //sequentialOutputPicture(picture);
                /*按照解码顺序输出*/
                sequenceOfDecodingOutputPicture(picture);
                picture->useFlag = false;
                picture = unoccupiedPicture;
            }

            picture->sliceHeader = sliceHeader;

            /*首先输出avc config*/
            if (avcFlag) {
                sequenceOfDecodingOutputPicture(picture);
                info.width = picture->sliceHeader.sps.PicWidthInSamplesL;
                info.height = picture->sliceHeader.sps.PicHeightInSamplesL;
                info.framerate = picture->sliceHeader.sps.fps;
                avcFlag = false;
            }

            if (sliceHeader.first_mb_in_slice == 0) {
                /*表示这帧正在使用，不要释放*/
                picture->useFlag = true;

                /*解码POC*/
                picture->decoding_process_for_picture_order_count();

                /*参考帧重排序在每个P、SP或B片的解码过程开始时调用。*/
                if (sliceHeader.slice_type == H264_SLIECE_TYPE_P ||
                    sliceHeader.slice_type == H264_SLIECE_TYPE_SP ||
                    sliceHeader.slice_type == H264_SLIECE_TYPE_B) {
                    gop.decoding_process_for_reference_picture_lists_construction(picture);
                }

            }

            picture->size += tempSize;
            picture->data.push_back({tempSize, buf, 0});

        } else if (nalUnitHeader.nal_unit_type == H264_NAL_SEI) {
            //++i;
        } else {
            fprintf(stderr, "不支持解析 nal_unit_type = %d", nalUnitHeader.nal_unit_type);
        }

    }
    /*刷新缓存里的帧*/
    // flushPicture(picture);
    /*输出最后一帧*/
    sequenceOfDecodingOutputPicture(picture);
    // double duration = frameNumber / sps.fps;
    info.videoDuration = (decodeFrameNumber + 1) / picture->sliceHeader.sps.fps;
    info.videodatarate = (double) videoFileSize / 1024 * 8 / info.videoDuration;
    return ret;
}


int Mux::flushFlv() {

    /*结束tag*/
    tagSize = FLVBody::flushVideo(fs);
    //tagSize = FLVBody::flushAudio(fs);
    const uint32_t length = bswap_32(tagSize);
    fs.write(reinterpret_cast<const char *>(&length), 4);
    info.filesize = static_cast<double>(fs.tellp());

    writeScriptTag();
    return 0;
}

int Mux::flushPicture(const NALPicture *picture) {
    sequentialOutputPicture(picture);

    const NALSliceHeader &sliceHeader = picture->sliceHeader;
    NALPicture *outPicture = nullptr;
    while (true) {
        getOutPicture(nullptr, outPicture, sliceHeader.sps.max_num_reorder_frames);
        if (outPicture == nullptr) {
            break;
        }

        sequenceOfDecodingOutputPicture(outPicture);

        outPicture->useFlag = false;
    }

    tagSize = FLVBody::flushVideo(fs);
    const uint32_t size = bswap_32(tagSize);
    fs.write(reinterpret_cast<const char *>(&size), 4);

    writeScriptTag();
    return 0;
}

int Mux::sequentialOutputPicture(const NALPicture *picture) {

    NALPicture *outPicture = nullptr;
    const NALSliceHeader &sliceHeader = picture->sliceHeader;

    if (sliceHeader.nalu.IdrPicFlag) { //idr帧
        /*输出所有缓存帧*/
        while (true) {
            getOutPicture(nullptr, outPicture, sliceHeader.sps.max_num_reorder_frames);
            if (outPicture == nullptr) {
                break;
            }

            sequenceOfDecodingOutputPicture(outPicture);

            outPicture->useFlag = false;
        }
    }
    getOutPicture(const_cast<NALPicture *>(picture), outPicture, sliceHeader.sps.max_num_reorder_frames);

    if (outPicture != nullptr) {
        sequenceOfDecodingOutputPicture(outPicture);
        outPicture->useFlag = false;
    }


    return 0;
}


int Mux::packaging(const char *filename) {

    fs.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!fs.is_open()) {
        fprintf(stderr, "cloud not open %s\n", filename);
        return -1;
    }
    writeFlvHeader();
    /*往后先偏移script tag的位置*/
    fs.seekp(4 + SCRIPT_TAG_SIZE, std::ios::cur);

    return 0;
}

int Mux::writeScriptTag() {
    /*跳过FLV header 还有PreviousTagSize，PreviousTagSize四个字节，最开始=0*/
    fs.seekp(9 + 4, std::ios::beg);

    tagSize = FLVBody::writeScript(fs, info);

    return 0;
}

int Mux::writeFlvHeader() {
    uint8_t FLVHeaderBuf[9];
    WriteStream ws(FLVHeaderBuf, 9);
    FLVHeader header;
    header.write(ws);

    fs.write(reinterpret_cast<const char *>(FLVHeaderBuf), 9);
    return 0;
}


int Mux::sequenceOfDecodingOutputPicture(const NALPicture *picture) {
    mux.lock();
    const uint32_t size = bswap_32(tagSize);
    fs.write(reinterpret_cast<const char *>(&size), 4);

    if (avcFlag) {
        tagSize = FLVBody::writeVideoConfig(picture, fs);
    } else {
        tagSize = FLVBody::writeVideo(picture, fs);
    }
    mux.unlock();
    return 0;
}

int Mux::sequenceOfDecodingOutputAudioFrame(const AdtsHeader &header) {
    mux.lock();
    const uint32_t size = bswap_32(tagSize);
    fs.write(reinterpret_cast<const char *>(&size), 4);

    if (aacFlag) {
        tagSize = FLVBody::writeAudioConfig(header, fs);
    } else {
        tagSize = FLVBody::writeAudio(header, fs);
    }

    mux.unlock();
    return 0;
}

int Mux::getOutPicture(NALPicture *picture, NALPicture *&outPicture, int max_num_reorder_frames) {


    /*直接输出,码流中没有b帧*/
    if (max_num_reorder_frames == 0) {
        outPicture = picture;
    }

    /*找出POC最小的一帧*/
    int idx = 0;
    for (int i = 1; i < pictureListSize; ++i) {
        if (pictureList[i]->pictureOrderCount < pictureList[idx]->pictureOrderCount) {
            idx = i;
        }
    }


    if (picture == nullptr) {  //表示IDR帧，要输出所有缓存
        if (pictureListSize > 0) {
            outPicture = pictureList[idx];


            --pictureListSize;
            for (int i = idx; i < pictureListSize; ++i) {
                pictureList[i] = pictureList[i + 1]; //数组元素整体向前移动一个单元
            }
        } else {
            outPicture = nullptr;
        }
    } else {
        if (pictureListSize < max_num_reorder_frames) {//说明pictureList[]数组还没塞满，只push，不get
            pictureList[pictureListSize++] = picture;
            outPicture = nullptr; //目前还没有可输出的帧
        } else {
            if (picture->pictureOrderCount < pictureList[idx]->pictureOrderCount) {
                outPicture = picture; //说明当前新输入的已解码帧的POC是最小的，一般为B帧
            } else {
                outPicture = pictureList[idx]; //输出DPB中的POC最小的那一帧
                pictureList[idx] = picture; //把新的一帧放进去
            }
        }
    }
    return 0;
}


Mux::~Mux() {
    fs.close();
}















