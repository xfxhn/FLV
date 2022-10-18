add_library(demux demux.cpp)


target_include_directories(demux
        PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream
        PRIVATE ${PROJECT_SOURCE_DIR}/src/FLVHeader
        PRIVATE ${PROJECT_SOURCE_DIR}/src/FLVFileBody
        )

#[[target_link_directories(demux
        PRIVATE ${PROJECT_SOURCE_DIR}/src/FLVFileBody
        PRIVATE ${PROJECT_SOURCE_DIR}/src/FLVHeader
        )]]
target_link_libraries(demux PRIVATE body header bitStream)