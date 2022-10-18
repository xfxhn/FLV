#ifndef DEMUX_ADTSHEADER_H
#define DEMUX_ADTSHEADER_H

#include <cstdint>

class WriteStream;

class AdtsHeader {
    /* 采样率 */
//    static int adts_sample_rates[16];
private:
    /* 帧同步标识一个帧的开始，固定为0xFFF */
    uint16_t syncword{0xFFF};
    /* MPEG 标示符。0表示MPEG-4，1表示MPEG-2 */
    uint8_t ID{0};
    /* 表示使用哪一层 11=Layer1 10=Layer2 01=Layer3 00=保留。参考mpeg-1 part3 2.4.2.3小节，这里是AAC，不兼容mpeg1音频，固定为00 */
    uint8_t layer{0};
    /*  是否在音频位流中添加了冗余以方便错误检测和隐藏(参见mpeg-1 part3 protection_bit)。标识是否进行误码校验。0表示有CRC校验，1表示没有CRC校验  */
    uint8_t protection_absent{1};
    /*
标识使用哪个级别的AAC
根据ID的值，如果ID等于' 1 '，该字段包含与ISO/IEC 13818-7中定义的ADTS流中的profile字段相同的信息。如果ID等于' 0 '，
此元素表示MPEG-4音频对象类型(profile_ObjectType+1)根据子条款1.5.2.1定义的表
profile_ObjectType | MPEG2 profile (ID=1) | MPEG-4 AOT (ID=0)
0                  | Main Profile         | AOT AAC MAIN (=1)
1                  | Low Complexity Profi | AOT AAC LC (=2)
2                  | Scalable Sampling Ra | AOT AAC SSR (=3)
3                  | (reserved)           | AOT AAC LTP (=4)
 */
    uint8_t profile{0};
    /* AudioObjectType objectType{AudioObjectType::LC}; */
    /* 标识使用的采样率的下标 */
    /* uint8_t sampling_frequency_index{0}; */
    uint8_t sample_rate_index{0};

    /* 私有位，编码时设置为0，解码时忽略
      指示是否在音频位流中添加了冗余以方便
      错误检测和隐藏。 如果没有添加冗余，则等于'1'，如果添加了冗余，则等于'0'
     */
    uint8_t private_bit{0};

    /* 标识声道数 通道配置
     * 如果如果channel_configuration大于0，则通道配置由表42中的' Default bitstream index number '给出，参见表8.5
     * 如果channel_configuration = 0,则通道配置未在标头中指定,
     * 并且必须由作为标头之后第一个 raw_data_block() 中的第一个句法元素的program_config_element() 或由隐式配置给出,
     * */
    uint8_t channel_configuration{0};
    /* 编码时设置为0，解码时忽略 */
    uint8_t original_copy{0};
    /* 编码时设置为0，解码时忽略 */
    uint8_t home{0};


    //可变header
    /* 编码时设置为0，解码时忽略 */
    uint8_t copyright_identification_bit{0};
    /* 编码时设置为0，解码时忽略 */
    uint8_t copyright_identification_start{0};
    /* ADTS帧长度包括ADTS长度和AAC声音数据长度的和。即 aac_frame_length = (protection_absent == 0 ? 9 : 7) + audio_data_length */
    uint16_t frame_length{0};
    /* 固定为0x7FF。表示是码率可变的码流 */
    uint16_t adts_buffer_fullness{0x7FF};
    /* 表示当前帧有number_of_raw_data_blocks_in_frame + 1 个原始帧(一个AAC原始帧包含一段时间内1024个采样及相关数据)。*/
    uint8_t number_of_raw_data_blocks_in_frame{0};
public:
    AdtsHeader(uint32_t audio_data_length, int profile, uint32_t sampleRate, int channels, int protection_absent = 1);

    int adts_fixed_header(WriteStream &ws) const;

    int adts_variable_header(WriteStream &ws) const;

private:
    int set_sample_rate_index(uint32_t sample_rate);
};

#endif //DEMUX_ADTSHEADER_H