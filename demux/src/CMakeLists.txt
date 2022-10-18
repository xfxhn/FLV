
#[[添加一个子目录并构建该子目录]]

add_subdirectory(demux)
add_subdirectory(bitStream)
add_subdirectory(decoderConfigurationRecord)
add_subdirectory(FLVHeader)
add_subdirectory(FLVFileBody)
add_subdirectory(adtsHeader)


add_executable(flv main.cpp)

target_include_directories(flv
        PRIVATE ${PROJECT_SOURCE_DIR}/src/demux
        #[[PRIVATE ${PROJECT_SOURCE_DIR}/src/bitStream]]
        )

target_link_libraries(flv PRIVATE demux)