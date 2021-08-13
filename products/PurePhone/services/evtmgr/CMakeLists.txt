add_library(PurePhone-evtmgr STATIC)

target_sources(PurePhone-evtmgr
    PRIVATE
        PureEventManager.cpp
        include/evtmgr/PureEventManager.hpp
)

target_include_directories(PurePhone-evtmgr
    PRIVATE
        include/evtmgr
    PUBLIC
        include
)

target_link_libraries(PurePhone-evtmgr
    PRIVATE
        module-bsp
        module-sys
        service-evtmgr
)
