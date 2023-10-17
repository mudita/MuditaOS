add_library(endpoint-message-common INTERFACE)

target_sources(
        endpoint-message-common
    INTERFACE
        include/endpoints/message/Common.hpp
)

target_include_directories(
        endpoint-message-common
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        endpoint-message-common
    INTERFACE
        json::json
)
