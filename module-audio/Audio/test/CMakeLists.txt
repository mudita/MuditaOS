cmake_minimum_required(VERSION 3.12)
# audio tests

add_catch2_executable(
    NAME
        audio-test
    SRCS
        unittest_audio.cpp
    LIBS
        module-vfs module-audio
)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testfiles" DESTINATION "${CMAKE_BINARY_DIR}")
