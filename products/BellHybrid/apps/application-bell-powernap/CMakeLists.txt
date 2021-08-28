# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-bell-powernap STATIC)

target_include_directories(application-bell-powernap
    PRIVATE
        $<BUILD_INTERFACE:
            include/application-bell-powernap
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-powernap
    PRIVATE
        ApplicationBellPowerNap.cpp
        presenter/PowerNapMainWindowPresenter.cpp
        windows/PowerNapMainWindow.cpp
        data/PowerNapListItem.cpp
        models/PowerNapModel.cpp

        presenter/PowerNapMainWindowPresenter.hpp
        windows/PowerNapMainWindow.hpp
        data/PowerNapListItem.hpp
        data/PowerNapStyle.hpp
        models/PowerNapModel.hpp

    PUBLIC
        include/application-bell-powernap/ApplicationBellPowerNap.hpp
)

target_link_libraries(application-bell-powernap
    PRIVATE

    PUBLIC
        apps-common
        module-gui
)
