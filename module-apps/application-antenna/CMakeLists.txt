# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-antenna STATIC)

target_include_directories(application-antenna

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

target_sources(application-antenna

    PRIVATE
        ApplicationAntenna.cpp
        windows/AntennaMainWindow.cpp
        windows/ScanModesWindow.cpp
        windows/AlgoParamsWindow.cpp
    PUBLIC
        ApplicationAntenna.hpp
)


target_compile_options(application-antenna
    PRIVATE
        $<$<CONFIG:DEBUG>: -Os>
)

target_link_libraries(application-antenna
    PRIVATE
        apps-common
        service-cellular
)

