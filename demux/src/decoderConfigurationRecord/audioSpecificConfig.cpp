#include "readStream.h"
#include "audioSpecificConfig.h"

int AudioSpecificConfig::parseData(ReadStream &rs) {


    audioObjectType = GetAudioObjectType(rs);
    /*采样率 Table 1.18 如果samplingFrequencyIndex等于15，那么实际的采样率直接由samplingFrequency的值表示 3是48000采样率*/
    samplingFrequencyIndex = rs.readMultiBit(4);

    if (samplingFrequencyIndex == 0xF) {
        samplingFrequency = rs.readMultiBit(24);
        sampleRate = samplingFrequency;
    } else {
        switch (samplingFrequencyIndex) {
            case 0x0:
                sampleRate = 96000;
                break;
            case 0x1:
                sampleRate = 88200;
                break;
            case 0x2:
                sampleRate = 64000;
                break;
            case 0x3:
                sampleRate = 48000;
                break;
            case 0x4:
                sampleRate = 44100;
                break;
            case 0x5:
                sampleRate = 32000;
                break;
            case 0x6:
                sampleRate = 24000;
                break;
            case 0x7:
                sampleRate = 22050;
                break;
            case 0x8:
                sampleRate = 16000;
                break;
            case 0x9:
                sampleRate = 12000;
                break;
            case 0xa:
                sampleRate = 11025;
                break;
            case 0xb:
                sampleRate = 8000;
                break;
            case 0xc:
                sampleRate = 7350;
                break;
            default:
                sampleRate = 0;
        }


    }

    /*通道数 参见Table 1.19 2是双通道*/
    channelConfiguration = rs.readMultiBit(4);



    /*是否开启了SBR和PS，这个是在mpeg-2 AAC上的扩展*/
    if (audioObjectType == 5 || audioObjectType == 29) {
        extensionAudioObjectType = 5;
        sbrPresentFlag = 1;
        if (audioObjectType == 29) {
            psPresentFlag = 1;
        }
        extensionSamplingFrequencyIndex = rs.readMultiBit(4);
        if (extensionSamplingFrequencyIndex == 0xF) {
            extensionSamplingFrequency = rs.readMultiBit(24);
        }
        audioObjectType = GetAudioObjectType(rs);
        if (audioObjectType == 22) {
            extensionChannelConfiguration = rs.readMultiBit(4);
        }
    } else {
        extensionAudioObjectType = 0;
    }

    switch (audioObjectType) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
        case 7:
        case 17:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
            GASpecificConfig(rs);
            break;
            /*case 8:
                //CelpSpecificConfig();
                break;
            case 9:
                //HvxcSpecificConfig();
                break;
            case 12:
                //TTSSpecificConfig();
                break;
            case 13:
            case 14:
            case 15:
            case 16:
                //StructuredAudioSpecificConfig();
                break;
            case 24:
                //ErrorResilientCelpSpecificConfig();
                break;
            case 25:
                //ErrorResilientHvxcSpecificConfig();
                break;
            case 26:
            case 27:
                //ParametricSpecificConfig();
                break;*/
            //...参考 1.6.2.1 AudioSpecificConfig
    }


    if (extensionAudioObjectType != 5 && rs.bitsToDecode() >= 16) {
        /*0x2b7表示HE-AAC的扩展*/
        syncExtensionType = rs.readMultiBit(11);
        if (syncExtensionType == 0x2b7) {
            extensionAudioObjectType = GetAudioObjectType(rs);
            if (extensionAudioObjectType == 5) {
                sbrPresentFlag = rs.readBit();
                if (sbrPresentFlag == 1) {
                    extensionSamplingFrequencyIndex = rs.readMultiBit(4);
                    if (extensionSamplingFrequencyIndex == 0xf) {
                        extensionSamplingFrequency = rs.readMultiBit(24);
                    }
                    if (rs.bitsToDecode() >= 12) {
                        syncExtensionType = rs.readMultiBit(11);
                        /*if (syncExtesionType == 0x548){
                            psPresentFlag = rs.readBit();
                        }*/
                    }
                }
            }
            if (extensionAudioObjectType == 22) {
                sbrPresentFlag = rs.readBit();
                if (sbrPresentFlag == 1) {
                    extensionSamplingFrequencyIndex = rs.readMultiBit(4);
                    if (extensionSamplingFrequencyIndex == 0xf) {
                        extensionSamplingFrequency = rs.readMultiBit(24);
                    }
                }
                extensionChannelConfiguration = rs.readMultiBit(4);
            }
        }
    }
    rs.byteAlignment();
    return 0;
}

uint8_t AudioSpecificConfig::GetAudioObjectType(ReadStream &rs) {
    uint8_t _audioObjectType = rs.readMultiBit(5);
    if (_audioObjectType == 31) {
        uint8_t audioObjectTypeExt = rs.readMultiBit(6);
        _audioObjectType = 32 + audioObjectTypeExt;
    }
    return _audioObjectType;
}

int AudioSpecificConfig::GASpecificConfig(ReadStream &rs) {
    /*帧的长度，光谱线的数量，分别对于除AAC SSR和ER AAC LD之外的所有通用音频对象类型*/
    /*当设置为0时，IMDCT为1024/128行，framength为1024;当设置为1时，IMDCT为960/120行，framength为960*/
    /*ER AAC LD:设置为0时，IMDCT为512行，framength为512;设置为1时，IMDCT为480行，framength为480*/
    /*对于AAC SSR:必须设置为0。使用256/32线IMDCT。注意:IMDCT的实际行数(第一个或第二个值)由window_sequence的值区分*/
    frameLengthFlag = rs.readBit();
    /*在可伸缩的AAC配置的底层中使用了核心编码器的信号*/
    dependsOnCoreCoder = rs.readBit();
    if (dependsOnCoreCoder) {
        /*在MDCT计算之前，必须应用于上采样(必要时)核心解码器输出的样本中的延迟*/
        uint16_t coreCoderDelay = rs.readMultiBit(14);
    }
    /*对于audioObjectType = 1、2、3、4、6、7，应为“0”。audioObjectType = 17,19,20,21,22,23应为' 1 '。*/
    extensionFlag = rs.readBit();
    //如果是0的话
    if (!channelConfiguration) {
        /*信道到扬声器映射*/
        /*在AOT相关的SpecificConfig中定义*/
        program_config_element(rs);
    }
    if ((audioObjectType == 6) || (audioObjectType == 20)) {
        /*表示可伸缩配置中的AAC层号。第一个AAC层由值0表示。*/
        uint8_t layerNr = rs.readMultiBit(3);
    }
    if (extensionFlag) {
        if (audioObjectType == 22) {
            uint8_t numOfSubFrame = rs.readMultiBit(5);

            uint16_t layer_length = rs.readMultiBit(11);
        }
        if (audioObjectType == 17 || audioObjectType == 19 ||
            audioObjectType == 20 || audioObjectType == 23) {
            bool aacSectionDataResilienceFlag = rs.readBit();
            bool aacScalefactorDataResilienceFlag = rs.readBit();
            bool aacSpectralDataResilienceFlag = rs.readBit();
        }

        bool extensionFlag3 = rs.readBit();
        if (extensionFlag3) {
            // todoList待定
        }
    }

    return 0;
}

int AudioSpecificConfig::program_config_element(ReadStream &rs) {

    uint8_t element_instance_tag = rs.readMultiBit(4);
    uint8_t object_type = rs.readMultiBit(2);
    uint8_t sampling_frequency_index = rs.readMultiBit(4);
    uint8_t num_front_channel_elements = rs.readMultiBit(4);
    uint8_t num_side_channel_elements = rs.readMultiBit(4);
    uint8_t num_back_channel_elements = rs.readMultiBit(4);
    uint8_t num_lfe_channel_elements = rs.readMultiBit(2);
    uint8_t num_assoc_data_elements = rs.readMultiBit(3);
    uint8_t num_valid_cc_elements = rs.readMultiBit(4);
    uint8_t mono_mixdown_present = rs.readMultiBit(1);
    if (mono_mixdown_present == 1) {
        uint8_t mono_mixdown_element_number = rs.readMultiBit(4);
    }
    uint8_t stereo_mixdown_present = rs.readMultiBit(1);
    if (stereo_mixdown_present == 1) {
        uint8_t stereo_mixdown_element_number = rs.readMultiBit(4);
    }
    uint8_t matrix_mixdown_idx_present = rs.readMultiBit(1);
    if (matrix_mixdown_idx_present == 1) {
        uint8_t matrix_mixdown_idx = rs.readMultiBit(2);
        uint8_t pseudo_surround_enable = rs.readMultiBit(1);
    }

    uint8_t *front_element_is_cpe = new uint8_t[num_front_channel_elements]; // NOLINT(modernize-use-auto)
    uint8_t *front_element_tag_select = new uint8_t[num_front_channel_elements]; // NOLINT(modernize-use-auto)
    for (int i = 0; i < num_front_channel_elements; ++i) {
        front_element_is_cpe[i] = rs.readMultiBit(1);
        front_element_tag_select[i] = rs.readMultiBit(4);
    }


    uint8_t *side_element_is_cpe = new uint8_t[num_side_channel_elements]; // NOLINT(modernize-use-auto)
    uint8_t *side_element_tag_select = new uint8_t[num_side_channel_elements]; // NOLINT(modernize-use-auto)
    for (int i = 0; i < num_side_channel_elements; ++i) {
        side_element_is_cpe[i] = rs.readMultiBit(1);
        side_element_tag_select[i] = rs.readMultiBit(4);
    }


    uint8_t *back_element_is_cpe = new uint8_t[num_back_channel_elements]; // NOLINT(modernize-use-auto)
    uint8_t *back_element_tag_select = new uint8_t[num_back_channel_elements]; // NOLINT(modernize-use-auto)
    for (int i = 0; i < num_back_channel_elements; ++i) {
        back_element_is_cpe[i] = rs.readMultiBit(1);
        back_element_tag_select[i] = rs.readMultiBit(4);
    }


    uint8_t *lfe_element_tag_select = new uint8_t[num_lfe_channel_elements]; // NOLINT(modernize-use-auto)
    for (int i = 0; i < num_lfe_channel_elements; ++i) {
        lfe_element_tag_select[i] = rs.readMultiBit(4);
    }


    uint8_t *assoc_data_element_tag_select = new uint8_t[num_assoc_data_elements]; // NOLINT(modernize-use-auto)
    for (int i = 0; i < num_assoc_data_elements; ++i) {
        assoc_data_element_tag_select[i] = rs.readMultiBit(4);
    }


    uint8_t *cc_element_is_ind_sw = new uint8_t[num_valid_cc_elements]; // NOLINT(modernize-use-auto)
    uint8_t *valid_cc_element_tag_select = new uint8_t[num_valid_cc_elements]; // NOLINT(modernize-use-auto)
    for (int i = 0; i < num_valid_cc_elements; ++i) {
        cc_element_is_ind_sw[i] = rs.readMultiBit(1);
        valid_cc_element_tag_select[i] = rs.readMultiBit(4);
    }
    rs.byteAlignment();
    uint8_t comment_field_bytes = rs.readMultiBit(8);

    uint8_t *comment_field_data = new uint8_t[comment_field_bytes]; // NOLINT(modernize-use-auto)
    for (int i = 0; i < comment_field_bytes; ++i) {
        comment_field_data[i] = rs.readMultiBit(8);
    }
    return 0;
}


