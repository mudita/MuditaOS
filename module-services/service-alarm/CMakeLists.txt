message( "service-alarm  ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(service-alarm STATIC)

set_target_properties(service-alarm PROPERTIES LINKER_LANGUAGE CXX)

target_sources(service-alarm
    PUBLIC
        include/service-alarm/AlarmMessage.hpp
        include/service-alarm/AlarmServiceAPI.hpp
    PRIVATE
        AlarmRepository.hpp
)

target_include_directories(service-alarm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            include
        >
)

target_link_libraries(service-alarm
    PUBLIC
        apps-common
        module-db
    PRIVATE
        messagetype
        module-sys
)
