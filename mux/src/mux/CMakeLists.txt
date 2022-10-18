add_library(mux mux.cpp)

target_include_directories(mux
        PRIVATE ${PROJECT_SOURCE_DIR}/src/FLVHeader
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        PUBLIC ${PROJECT_SOURCE_DIR}/src/nalu
        )
target_link_libraries(mux PRIVATE header bitStream nalu)