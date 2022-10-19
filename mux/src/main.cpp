

#include "mux.h"

void ebsp_to_rbsp(uint8_t *data, uint32_t size) {
    uint32_t NumBytesInRBSP = 0;
    for (uint32_t i = 0; i < size; i++) {
        if ((i + 2 < size) && data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 3) {
            data[NumBytesInRBSP++] = data[i];
            data[NumBytesInRBSP++] = data[i + 1];
            i += 2;
        } else {
            data[NumBytesInRBSP++] = data[i];
        }
    }
}

int main() {

    /*uint8_t arr[11] = {21, 3, 0, 0, 3, 12, 5, 5, 0, 0, 3,};

    ebsp_to_rbsp(arr, 11);*/


    Mux mux;


    mux.initAudio("resource/ouput1.aac");
    mux.initVideo("resource/ouput.h264");

    mux.packaging("resource/test.flv");
    return 0;
}
