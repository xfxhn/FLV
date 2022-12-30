

#include "mux.h"

#include <thread>

int main() {


    int ret;
    Mux mux;
    /*ouput.h264*/

    ret = mux.packaging("resource/test.flv");
    if (ret < 0) {
        return ret;
    }


    std::thread th1(&Mux::initVideo, &mux, "resource/slice3gop2.h264");
    std::thread th2(&Mux::initAudio, &mux, "resource/ouput1.aac");


    th1.join();
    th2.join();

    mux.flushFlv();

    return 0;
}
