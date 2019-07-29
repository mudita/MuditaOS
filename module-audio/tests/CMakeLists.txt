cmake_minimum_required(VERSION 3.12)


project(unittest_module-audio VERSION 1.0
        DESCRIPTION "Audio module library unit tests")




add_executable( ${PROJECT_NAME}

        "${CMAKE_CURRENT_SOURCE_DIR}/tests-main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Playback_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Recorder_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Decoders_tests.cpp"
        #"${CMAKE_CURRENT_SOURCE_DIR}/StateMachine_tests.cpp"
        )

target_link_libraries( ${PROJECT_NAME} PRIVATE module-audio portaudio )

