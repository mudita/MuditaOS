add_library(desktop-endpoints-product STATIC)

target_sources(
        desktop-endpoints-product
    PRIVATE
        EndpointFactoryBell.hpp
        EndpointFactoryBell.cpp
        deviceInfo/DeviceInfoEndpoint.cpp
    PUBLIC
        include/endpoints/deviceInfo/DeviceInfoEndpoint.hpp
)

target_include_directories(
        desktop-endpoints-product
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        desktop-endpoints-product
    PUBLIC
        desktop-endpoints-common
    PRIVATE
        version-header
        serial-number-reader
)

target_compile_definitions(
        desktop-endpoints-product
    PRIVATE
        ENABLE_DEVELOPER_MODE_ENDPOINT=$<BOOL:${ENABLE_DEVELOPER_MODE_ENDPOINT}>
)
