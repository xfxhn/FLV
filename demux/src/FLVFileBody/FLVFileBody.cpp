#include <cstdio>
#include "FLVFileBody.h"
#include "readStream.h"

#include "scriptTag.h"
#include "videoTag.h"
#include "audioTag.h"

int FLVFileBody::parseHeader(std::ifstream &fs, std::ofstream &videoOutFile, std::ofstream &audioOutFile) {
    uint8_t buffer[11];
    fs.read(reinterpret_cast<char *>(buffer), 11);
    ReadStream rs(buffer, 11);


    tagHeader.parseData(rs);
    if (tagHeader.Filter) {
        fprintf(stderr, "non-supported Encrypted\n");
        return -1;
    }


    if (tagHeader.TagType == 8) {
        //printf("audio\n");
        AudioTag tag{};
        tag.parseData(fs, tagHeader.DataSize, audioOutFile);
    } else if (tagHeader.TagType == 9) {
        //printf("video\n");
        VideoTag tag{};
        tag.parseData(fs, tagHeader.DataSize, videoOutFile, tagHeader.realTimestamp);
    } else if (tagHeader.TagType == 18) {
        ScriptTag tag;
        tag.parseData(fs, tagHeader.DataSize);
        tag.printData();
    } else {

        fprintf(stderr, "this is tagType non-supported %d\n ", tagHeader.TagType);
        return -1;
    }

    return 0;
}
