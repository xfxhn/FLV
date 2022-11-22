


add_library(body FLVBody.cpp tagHeader.cpp scriptTag.cpp videoTag.cpp audioTag.cpp)

target_include_directories(body
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        PRIVATE ${PROJECT_SOURCE_DIR}/src/nalu
        PRIVATE ${PROJECT_SOURCE_DIR}/src/decoderConfigurationRecord
        PRIVATE ${PROJECT_SOURCE_DIR}/src/adts
        PRIVATE ${PROJECT_SOURCE_DIR}/src/mux
        )

target_link_libraries(mux PRIVATE decoderConfigurationRecord)