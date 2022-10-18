
add_subdirectory(bitStream)
add_subdirectory(nalu)
add_subdirectory(mux)
add_subdirectory(FLVHeader)


add_executable(flv main.cpp)

message(STATUS ${PROJECT_SOURCE_DIR}/mux1)
target_include_directories(flv PRIVATE ${PROJECT_SOURCE_DIR}/src/mux)
target_link_libraries(flv PRIVATE mux)