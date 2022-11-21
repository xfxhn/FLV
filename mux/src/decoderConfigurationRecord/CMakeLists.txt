

add_library(decoderConfigurationRecord audioSpecificConfig.cpp AVCDecoderConfigurationRecord.cpp)

target_include_directories(decoderConfigurationRecord
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        PRIVATE ${PROJECT_SOURCE_DIR}/src/nalu
        PRIVATE ${PROJECT_SOURCE_DIR}/src/adts
        )