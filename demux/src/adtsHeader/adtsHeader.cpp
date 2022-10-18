

#include "adtsHeader.h"
#include "writeStream.h"
#include "readStream.h"


int AdtsHeader::adts_fixed_header(WriteStream &ws) const {
    ReadStream rs(nullptr, 10);
    ws.writeMultiBit(12, syncword);
    ws.writeMultiBit(1, ID);
    ws.writeMultiBit(2, layer);
    ws.writeMultiBit(1, protection_absent);
    ws.writeMultiBit(2, profile);
    ws.writeMultiBit(4, sample_rate_index);
    ws.writeMultiBit(1, private_bit);
    ws.writeMultiBit(3, channel_configuration);
    ws.writeMultiBit(1, original_copy);
    ws.writeMultiBit(1, home);
    return 0;
}

AdtsHeader::AdtsHeader(uint32_t audio_data_length, const int profile, const uint32_t sampleRate, const int channels,
                       int _protection_absent) {
    protection_absent = _protection_absent;


    set_sample_rate_index(sampleRate);
    this->profile = profile;
    channel_configuration = channels;
    frame_length = (protection_absent == 0 ? 9 : 7) + audio_data_length;
}


int AdtsHeader::adts_variable_header(WriteStream &ws) const {
    ws.writeMultiBit(1, copyright_identification_bit);
    ws.writeMultiBit(1, copyright_identification_start);
    ws.writeMultiBit(13, frame_length);
    ws.writeMultiBit(11, adts_buffer_fullness);
    ws.writeMultiBit(2, number_of_raw_data_blocks_in_frame);
    return 0;
}

int AdtsHeader::set_sample_rate_index(const uint32_t sample_rate) {
    if (sample_rate >= 92017) {
        sample_rate_index = 0;//96000
    } else if (sample_rate < 92017 && sample_rate >= 75132) {
        sample_rate_index = 1;//88200
    } else if (sample_rate < 75132 && sample_rate >= 55426) {
        sample_rate_index = 2;//64000
    } else if (sample_rate < 55426 && sample_rate >= 46009) {
        sample_rate_index = 3;//48000
    } else if (sample_rate < 46009 && sample_rate >= 37566) {
        sample_rate_index = 4;//44100
    } else if (sample_rate < 37566 && sample_rate >= 27713) {
        sample_rate_index = 5;//32000
    } else if (sample_rate < 27713 && sample_rate >= 23004) {
        sample_rate_index = 6;//24000
    } else if (sample_rate < 23004 && sample_rate >= 18783) {
        sample_rate_index = 7;//22050
    } else if (sample_rate < 18783 && sample_rate >= 13856) {
        sample_rate_index = 8;//16000
    } else if (sample_rate < 13856 && sample_rate >= 11502) {
        sample_rate_index = 9;//12000
    } else if (sample_rate < 11502 && sample_rate >= 9391) {
        sample_rate_index = 10;//11025
    } else if (sample_rate < 9391) {
        sample_rate_index = 11;//8000
    }
    return 0;
}