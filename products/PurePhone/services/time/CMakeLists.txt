add_library(pure-time STATIC)
add_library(pure::time ALIAS pure-time)

target_sources(pure-time
    PRIVATE
        AlarmOperations.cpp
    PUBLIC
        include/time/AlarmOperations.hpp
)

target_include_directories(pure-time
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pure-time
    PRIVATE
        module-sys
        pure::alarms
    PUBLIC
        service-time
)
