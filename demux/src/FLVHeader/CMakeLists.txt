add_library(header FLVHeader.cpp)


target_include_directories(header
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        )