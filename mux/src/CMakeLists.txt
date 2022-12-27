
add_subdirectory(bitStream)
add_subdirectory(nalu)
add_subdirectory(adts)
add_subdirectory(mux)
add_subdirectory(FLVHeader)
add_subdirectory(FLVBody)
add_subdirectory(decoderConfigurationRecord)


add_executable(flv main.cpp)

message(STATUS ${PROJECT_SOURCE_DIR}/mux1)
target_include_directories(flv
        PRIVATE ${PROJECT_SOURCE_DIR}/src/mux
        )
target_link_libraries(flv PRIVATE mux)