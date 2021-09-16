add_library(bell-time STATIC)
add_library(bell::time ALIAS bell-time)

target_sources(bell-time
    PRIVATE
        AlarmOperations.cpp
    PUBLIC
        include/time/AlarmOperations.hpp
)

target_include_directories(bell-time
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(bell-time
    PRIVATE
        module-sys
        bell::alarms
    PUBLIC
        service-time
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
