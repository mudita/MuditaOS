add_library(endpoint-message-sender STATIC)

target_sources(
        endpoint-message-sender
    PRIVATE
        Sender.cpp
    PUBLIC
        include/endpoints/message/Sender.hpp
)

target_include_directories(
        endpoint-message-sender
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        endpoint-message-sender
    PUBLIC
        endpoint-message-common
        module-os
)
