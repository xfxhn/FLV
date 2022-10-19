#include "mux.h"

#include <list>

#include "FLVHeader.h"
#include "writeStream.h"
#include "readStream.h"
#include "NALReader.h"
#include "NALHeader.h"
#include "adtsReader.h"
#include "adtsHeader.h"


struct tag {

};


enum NalUintType {
    H264_NAL_UNSPECIFIED = 0,
    H264_NAL_SLICE = 1,
    H264_NAL_IDR_SLICE = 5,
    H264_NAL_SEI = 6,
    H264_NAL_SPS = 7,
    H264_NAL_PPS = 8
};

int Mux::initAudio(const char *filename) {
    int ret;

    AdtsReader reader;
    ret = reader.init(filename);
    if (ret < 0) {
        fprintf(stderr, "init Audio failed\n");
        return ret;
    }

    AdtsHeader header;
    uint8_t *data = nullptr;
    bool stopFlag = true;

    int num = 0;
    while (stopFlag) {
        ret = reader.adts_sequence(header, data, stopFlag);
        if (ret < 0) {
            fprintf(stderr, "解析adts_sequence 失败\n");
            return ret;
        }
        ++num;

    }
    return ret;
}


int Mux::initVideo(const char *filename) {
    int ret;
    NaluReader nalUint;
    ret = nalUint.init(filename);
    if (ret < 0) {
        fprintf(stderr, "init video failed\n");
        return ret;
    }
    NALHeader header;


    uint8_t *data = nullptr;
    uint32_t size = 0;
    bool isStopLoop = true;
    uint32_t frameNumber = 0;
    while (isStopLoop) {
        nalUint.readNalUint(data, size, isStopLoop);


        header.nal_unit(data[0]);
        if (header.nal_unit_type == H264_NAL_SPS) {
            printf("sps\n");
            NALHeader::ebsp_to_rbsp(data, size);
            ReadStream rs(data, size);
            sps.seq_parameter_set_data(rs);

        } else if (header.nal_unit_type == H264_NAL_PPS) {
            printf("pps\n");
            NALHeader::ebsp_to_rbsp(data, size);
        } else if (header.nal_unit_type == H264_NAL_SLICE) {
            ++frameNumber;
        } else if (header.nal_unit_type == H264_NAL_IDR_SLICE) {
            printf("idr\n");
            ++frameNumber;
        } else if (header.nal_unit_type == H264_NAL_SEI) {
            //++i;
        } else {
            int a = 1;
        }

    }
    double duration = frameNumber / sps.fps;

    return ret;
}

int Mux::packaging(const char *filename) {

    fs.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!fs.is_open()) {
        fprintf(stderr, "cloud not open %s\n", filename);
        return -1;
    }
    writeFlvHeader();


    return 0;
}


int Mux::writeFlvHeader() {
    uint8_t FLVHeaderBuf[9]{0};
    WriteStream ws(FLVHeaderBuf, 9);
    FLVHeader header;
    header.write(ws);

    fs.write(reinterpret_cast<const char *>(FLVHeaderBuf), 7);
    return 0;
}

Mux::~Mux() {
    fs.close();
}


