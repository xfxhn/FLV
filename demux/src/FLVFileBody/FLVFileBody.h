
#ifndef FLV_FLVFILEBODY_H
#define FLV_FLVFILEBODY_H

#include <fstream>
#include "tagHeader.h"


class FLVFileBody {
public:
    int parseHeader(std::ifstream &fs, std::ofstream &videoOutFile, std::ofstream &audioOutFile);

private:
    TagHeader tagHeader;
};


#endif //FLV_FLVFILEBODY_H
