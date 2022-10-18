

add_library(nalu NALReader.cpp NALHeader.cpp NALSeqParameterSet.cpp)


target_include_directories(nalu
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        )
