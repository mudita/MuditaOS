add_library(evtmgr STATIC)

target_sources(evtmgr
    PRIVATE
        EventManager.cpp
    PUBLIC
        include/evtmgr/EventManager.hpp
)

target_include_directories(evtmgr
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(evtmgr
   PRIVATE
        module-utils
        service-evtmgr
)
