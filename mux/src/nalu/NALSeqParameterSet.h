
#ifndef MUX_NALSEQPARAMETERSET_H
#define MUX_NALSEQPARAMETERSET_H

// 7.4.2.1.1: num_ref_frames_in_pic_order_cnt_cycle的范围是0到255(包括0和255)。
#define H264_MAX_OFFSET_REF_FRAME_COUNT 256

#include <cstdint>


class ReadStream;

class NALSeqParameterSet {

public:
    uint8_t profile_idc; // 0 u(8)
    bool constraint_set0_flag; // 0 u(1)
    bool constraint_set1_flag; // 0 u(1)
    bool constraint_set2_flag; // 0 u(1)
    bool constraint_set3_flag; // 0 u(1)
    bool constraint_set4_flag; // 0 u(1)
    bool constraint_set5_flag; // 0 u(1)

    uint8_t level_idc; // 0 u(8)
    uint32_t seq_parameter_set_id; // 0 ue(v)
    uint8_t chroma_format_idc;   // 0 ue(v)
    bool separate_colour_plane_flag;  // 0 u(1)
    uint8_t bit_depth_luma_minus8;   // 0 ue(v)当bit_depth_luma_minus8不存在时，它将被推断为等于0。bit_depth_luma_minus8的取值范围为0到6(包括6)。
    uint8_t bit_depth_chroma_minus8;   // 0 ue(v)
    bool qpprime_y_zero_transform_bypass_flag{false};  //没有特别指定时，应推定其值为0
    bool seq_scaling_matrix_present_flag;  // 0 u(1)
    /*bool seq_scaling_list_present_flag[12];*/
    uint8_t log2_max_frame_num_minus4;  // 0-12
    uint16_t MaxFrameNum;
    uint8_t pic_order_cnt_type;  // 0 ue(v)
    uint8_t log2_max_pic_order_cnt_lsb_minus4;  // 0 ue(v)
    uint32_t MaxPicOrderCntLsb;
    bool delta_pic_order_always_zero_flag;  // 0 ue(v)
    int offset_for_non_ref_pic;  //-2^31-1 - 2^31-1，有符号4字节整数
    int offset_for_top_to_bottom_field;   // 0 se(v)
    uint8_t num_ref_frames_in_pic_order_cnt_cycle; // 0-255
    int offset_for_ref_frame[H264_MAX_OFFSET_REF_FRAME_COUNT]; //-2^31-1 - 2^31-1，有符号4字节整数
    int ExpectedDeltaPerPicOrderCntCycle{0}; //offset_for_ref_frame[ num_ref_frames_in_pic_order_cnt_cycle ] 0 se(v)

    uint8_t max_num_ref_frames; // 0 ue(v)
    bool gaps_in_frame_num_value_allowed_flag; // 0 u(1)
    uint32_t pic_width_in_mbs_minus1; // 0 ue(v)
    uint32_t pic_height_in_map_units_minus1; // 0 ue(v)
    bool frame_mbs_only_flag; // 0 u(1)
    bool mb_adaptive_frame_field_flag; // 0 u(1)
    bool direct_8x8_inference_flag; // 0 u(1)
    bool frame_cropping_flag; // 0 u(1)
    uint32_t frame_crop_left_offset; // 0 ue(v)
    uint32_t frame_crop_right_offset; // 0 ue(v)
    uint32_t frame_crop_top_offset; // 0 ue(v)
    uint32_t frame_crop_bottom_offset; // 0 ue(v)



    uint32_t PicWidthInMbs{0};
    uint32_t PicHeightInMapUnits{0}; //PicHeightInMapUnits = pic_height_in_map_units_minus1 + 1;
    uint32_t PicSizeInMapUnits{0};
    uint8_t ChromaArrayType{0};
    uint32_t PicWidthInSamplesL{0};
    uint32_t PicHeightInSamplesL{0};

    bool vui_parameters_present_flag; // 0 u(1)


    //辅助信息

    double fps{25.0};
    uint8_t aspect_ratio_idc{0};
    //长宽比
    uint16_t sar_width{0};
    uint16_t sar_height{0};
    bool overscan_appropriate_flag{false};

    uint8_t video_format{5};
    bool video_full_range_flag{0};
    uint8_t colour_primaries;
    uint8_t transfer_characteristics;
    uint8_t matrix_coefficients;

    //取值范围为0 ~ 5，包括0 ~ 5
    uint8_t chroma_sample_loc_type_top_field;
    uint8_t chroma_sample_loc_type_bottom_field;

    bool timing_info_present_flag{false};
    uint32_t num_units_in_tick{0};
    uint32_t time_scale{0};
    bool fixed_frame_rate_flag{false};


    bool low_delay_hrd_flag{false};
public:
    int seq_parameter_set_data(ReadStream &rs);

    int vui_parameters(ReadStream &rs);
};


#endif //MUX_NALSEQPARAMETERSET_H
