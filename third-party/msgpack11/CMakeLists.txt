add_library(msgpack11 msgpack11/msgpack11.cpp)

target_include_directories(msgpack11
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
