

#include "mux.h"


#include "readStream.h"
#include "writeStream.h"

#include <cstring>

#include <thread>
#include <mutex>


int main() {

    int ret;
    Mux mux;
/*ouput.h264*/

    ret = mux.packaging("resource/test.flv");
    if (ret < 0) {
        return ret;
    }


    std::thread th1(&Mux::initVideo, &mux, "resource/ouput.h264");
    std::thread th2(&Mux::initAudio, &mux, "resource/ouput1.aac");


    th1.join();
    th2.join();

    mux.flushFlv();
    /*ret = mux.initAudio("resource/ouput1.aac");
    if (ret < 0) {
        return ret;
    }*/

    /*ret = mux.initVideo("resource/slice3gop2.h264");
    if (ret < 0) {
        return ret;
    }*/

    return 0;
}
