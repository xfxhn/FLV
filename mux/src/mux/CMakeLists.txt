add_library(mux mux.cpp)

target_include_directories(mux
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        PRIVATE ${PROJECT_SOURCE_DIR}/src/FLVHeader
        PRIVATE ${PROJECT_SOURCE_DIR}/src/FLVBody
        PUBLIC ${PROJECT_SOURCE_DIR}/src/nalu
        PUBLIC ${PROJECT_SOURCE_DIR}/src/adts
        )
target_link_libraries(mux PRIVATE bitStream header body nalu adts)