add_library(service-antenna STATIC)

target_sources(service-antenna
    PRIVATE
        api/AntennaServiceAPI.cpp
        ServiceAntenna.cpp
    PUBLIC
        include/service-antenna/AntennaMessage.hpp
        include/service-antenna/AntennaServiceAPI.hpp
        include/service-antenna/ServiceAntenna.hpp
        include/service-antenna/ServiceState.hpp
)

target_link_libraries(service-antenna
    PRIVATE
        eventstore
        module-apps
        module-cellular
        module-utils
        service-cellular
    PUBLIC
        messagetype
        module-bsp
        module-sys
        service-db
)

target_include_directories(service-antenna
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/service-antenna>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
