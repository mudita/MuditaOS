# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-antenna STATIC)

target_include_directories(application-antenna
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-antenna

    PRIVATE
        ApplicationAntenna.cpp
        windows/AlgoParamsWindow.cpp
        windows/AntennaMainWindow.cpp
        windows/ScanModesWindow.cpp
    PRIVATE
        AntennaAppStyle.hpp
        windows/AlgoParamsWindow.hpp
        windows/AntennaMainWindow.hpp
        windows/ScanModesWindow.hpp
    PUBLIC
        include/application-antenna/ApplicationAntenna.hpp
)

target_link_libraries(application-antenna
    PRIVATE
        apps-common
        service-antenna
        service-cellular
)

