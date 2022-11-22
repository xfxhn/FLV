
#ifndef MUX_MUX_H
#define MUX_MUX_H

#include <fstream>
#include <mutex>

#include "NALSeqParameterSet.h"
#include "NALPictureParameterSet.h"
#include "NALDecodedPictureBuffer.h"


#define SCRIPT_TAG_SIZE 424

class ReadStream;

class AdtsHeader;

struct ScriptInfo {
    double videoDuration{0};
    double audioDuration{0};
    double width{0};
    double height{0};
    double videodatarate{0};
    double framerate{0};
    double audiodatarate{0};
    double audiosamplerate{0};
    double audiosamplesize{0};
    bool stereo{true};
    double filesize{0};
};

class Mux {


    /*script信息*/
private:
    ScriptInfo info;
private:


    std::mutex mux;
    uint32_t tagSize{SCRIPT_TAG_SIZE};

    /*最多缓存16张图片*/
    NALPicture *pictureList[16]{nullptr};
    /*具体缓存了多少张图片*/
    uint8_t pictureListSize{0};


    std::ofstream fs;

    NALSeqParameterSet spsList[32];
    NALPictureParameterSet ppsList[256];

    bool avcFlag{true};
    bool aacFlag{true};
public:
    int initAudio(const char *filename);

    int initVideo(const char *filename);

    int packaging(const char *filename);

    int flushFlv();

    ~Mux();

private:
    /*FLV的header*/
    int writeFlvHeader();

    int writeScriptTag();

    int sequentialOutputPicture(const NALPicture *picture);

    int getOutPicture(NALPicture *picture, NALPicture *&outPicture, int max_num_reorder_frames);

    int sequenceOfDecodingOutputPicture(const NALPicture *picture);

    int sequenceOfDecodingOutputAudioFrame(const AdtsHeader &header);

    int flushPicture(const NALPicture *picture);


    static int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq);
};


#endif //MUX_MUX_H
