add_library(tar tar.cpp)
target_link_libraries(tar PUBLIC microtar)
target_include_directories(tar PUBLIC include)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif()