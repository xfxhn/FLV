
#ifndef MUX_MUX_H
#define MUX_MUX_H

#include <fstream>

#include "NALSeqParameterSet.h"

class ReadStream;

class Mux {
private:
    std::ofstream fs;


    NALSeqParameterSet sps;
public:
    int initAudio(const char *filename);

    int initVideo(const char *filename);

    int packaging(const char *filename);

    ~Mux();

private:
    int writeFlvHeader();

};


#endif //MUX_MUX_H
