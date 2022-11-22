
#include "adtsHeader.h"

#include <cstdio>
#include "readStream.h"

enum AudioObjectType {
    Main = 0,
    LC = 1,
    SSR = 2,
    LTP = 3,
    LD = 23
};
constexpr static uint32_t adts_sample_rates[] = {
        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, 0, 0, 0
};

int AdtsHeader::adts_fixed_header(ReadStream &rs) {
    syncword = rs.readMultiBit(12);
    ID = rs.readBit();
    layer = rs.readMultiBit(2);
    protection_absent = rs.readMultiBit(1);
    if (protection_absent == 0) {
        fprintf(stderr, "不支持CRC校验\n");
        return -1;
    }
    profile = rs.readMultiBit(2);
    sampling_frequency_index = rs.readMultiBit(4);
    sample_rate = adts_sample_rates[sampling_frequency_index];
    private_bit = rs.readBit();
    channel_configuration = rs.readMultiBit(3);
    original_copy = rs.readBit();
    home = rs.readBit();

    return 0;
}

int AdtsHeader::adts_variable_header(ReadStream &rs) {
    copyright_identification_bit = rs.readBit();
    copyright_identification_start = rs.readBit();
    frame_length = rs.readMultiBit(13);
    adts_buffer_fullness = rs.readMultiBit(11);
    number_of_raw_data_blocks_in_frame = rs.readMultiBit(2);
    return 0;
}
