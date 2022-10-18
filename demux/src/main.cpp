#include "demux.h"


/*#include "readStream.h"*/

int main() {

    Demux de("resource/input.flv");
    int ret = de.init();

    if (ret < 0) {
        return ret;
    }


    de.demux();
    return 0;
}
