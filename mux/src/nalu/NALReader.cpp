
#include "NALReader.h"


int NaluReader::init(const char *filename) {
    fs.open(filename, std::ios::out | std::ios::binary);

    if (!fs.is_open()) {
        fprintf(stderr, "open %s failed\n", filename);
        return -1;
    }
    bufferStart = new uint8_t[MAX_BUFFER_SIZE];
    fs.read(reinterpret_cast<char *>(bufferStart), MAX_BUFFER_SIZE);
    blockBufferSize = fs.gcount();

    bufferPosition = bufferStart;
    bufferEnd = bufferStart + blockBufferSize - 1;

    return 0;
}

int NaluReader::readNalUint(uint8_t *&data, uint32_t &size, bool &isStopLoop) {

    while (true) {
        uint8_t *pos1 = nullptr;
        uint8_t *pos2 = nullptr;
        int startCodeLen1 = 0;
        int startCodeLen2 = 0;

        const int type = getNextStartCode(bufferPosition, bufferEnd,
                                          pos1, pos2, startCodeLen1, startCodeLen2);
        /*还剩多少字节未读取*/
        uint32_t residual = (bufferEnd - pos1 + 1);
        /*已经读取了多少个字节*/
        uint32_t readSize = blockBufferSize - residual;
        if (type == 1) { //表示找到了开头的startCode,没找到后面的
            memcpy(bufferStart, pos1, residual);
            //每次读File::MAX_BUFFER_SIZE个，这里读取的NALU必须要包含一整个slice,字节对齐
            //size_t bufferSize = fread(bufferStart + residual, 1, readSize, file);
            fs.read(reinterpret_cast<char *>(bufferStart + residual), readSize);
            uint32_t bufferSize = fs.gcount();


            if (bufferSize == 0) {
                //表示读完数据了

                /* uint8_t *EBSP = bufferStart + startCodeLen1;
                 size = EBSP_TO_RBSP(EBSP, residual - startCodeLen1);
                 data = EBSP;*/
                size = residual - startCodeLen1;
                data = bufferStart + startCodeLen1;
                isStopLoop = false;
                break;
            }
            blockBufferSize = residual + bufferSize;
            bufferPosition = bufferStart;
            bufferEnd = bufferStart + blockBufferSize - 1;
        } else if (type == 2) {//都找到了

            data = pos1 + startCodeLen1;
            size = pos2 - data;
            /* uint8_t *EBSP = pos1 + startCodeLen1;
             size_t EBSPSize = pos2 - EBSP;
             size = EBSP_TO_RBSP(EBSP, EBSPSize);
             data = EBSP;*/
            bufferPosition = pos2;// data + size;
            break;
        } else {
            //错误
            fprintf(stderr, "没有找到开头startCode，也没有找到后面的startCode\n");
            isStopLoop = false;
            return -1;
        }
    }
    return 0;
}

int NaluReader::getNextStartCode(uint8_t *bufPtr, const uint8_t *end,
                                 uint8_t *&pos1, uint8_t *&pos2, int &startCodeLen1, int &startCodeLen2) {
    uint8_t *pos = bufPtr;
    int type = 0;
    //查找开头的startCode
    while (pos < end) {
        if (pos[0] == 0) {
            if (pos[1] == 0) {
                if (pos[2] == 0) {
                    if (pos[3] == 1) { //0 0 0 1
                        startCodeLen1 = 4;
                        pos1 = pos;
                        pos += startCodeLen1;
                        type = 1;
                        break;

                    }
                } else if (pos[2] == 1) {  //0 0 1
                    startCodeLen1 = 3;
                    pos1 = pos;
                    pos += startCodeLen1;
                    type = 1;
                    break;
                }
            }
        }
        pos++;
    }
    //查找结尾的startCode
    while (pos < end) {
        if (pos[0] == 0) {
            if (pos[1] == 0) {
                if (pos[2] == 0) {
                    if (pos[3] == 1) { //0 0 0 1
                        pos2 = pos;
                        startCodeLen2 = 4;
                        type = 2;
                        break;
                    }
                } else if (pos[2] == 1) {  //0 0 1
                    pos2 = pos;
                    startCodeLen2 = 3;
                    type = 2;
                    break;
                }

            }
        }
        pos++;
    }
    return type;
}

NaluReader::~NaluReader() {
    fs.close();
    if (bufferStart) {
        delete[] bufferStart;
        bufferStart = nullptr;
    }
}
