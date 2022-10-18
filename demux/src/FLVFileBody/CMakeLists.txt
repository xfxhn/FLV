


add_library(body FLVFileBody.cpp tagHeader.cpp scriptTag.cpp videoTag.cpp audioTag.cpp)


target_include_directories(body
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        PRIVATE ${PROJECT_SOURCE_DIR}/src/decoderConfigurationRecord
        PRIVATE ${PROJECT_SOURCE_DIR}/src/adtsHeader
        )

target_link_libraries(body PRIVATE decoderConfigurationRecord adtsHeader)