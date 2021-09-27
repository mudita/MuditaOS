add_library(desktop-endpoints-product STATIC)

target_sources(
        desktop-endpoints-product
    PRIVATE
        EndpointFactoryPure.hpp
        EndpointFactoryPure.cpp
        calllog/CalllogEndpoint.cpp
        calllog/CalllogHelper.cpp
        contacts/ContactHelper.cpp
        contacts/ContactsEndpoint.cpp
        deviceInfo/DeviceInfoEndpoint.cpp
        messages/MessageHelper.cpp
        messages/MessagesEndpoint.cpp
    PUBLIC
        include/endpoints/calllog/CalllogEndpoint.hpp
        include/endpoints/calllog/CalllogHelper.hpp
        include/endpoints/contacts/ContactHelper.hpp
        include/endpoints/contacts/ContactsEndpoint.hpp
        include/endpoints/deviceInfo/DeviceInfoEndpoint.hpp
        include/endpoints/messages/MessageHelper.hpp
        include/endpoints/messages/MessagesEndpoint.hpp
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
)

target_compile_definitions(
        desktop-endpoints-product
    PRIVATE
        ENABLE_DEVELOPER_MODE_ENDPOINT=$<BOOL:${ENABLE_DEVELOPER_MODE_ENDPOINT}>
)
