# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-bell-powernap STATIC)
add_library(bell::app-powernap ALIAS application-bell-powernap)

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
        presenter/PowerNapProgressPresenter.cpp
        presenter/PowerNapSessionEndedPresenter.cpp
        widgets/PowerNapAlarm.cpp
        windows/PowerNapMainWindow.cpp
        windows/PowerNapProgressWindow.cpp
        windows/PowerNapSessionEndedWindow.cpp
        data/PowerNapListItem.cpp
        models/PowerNapModel.cpp

        presenter/PowerNapMainWindowPresenter.hpp
        presenter/PowerNapProgressPresenter.hpp
        presenter/PowerNapSessionEndedPresenter.hpp
        widgets/PowerNapAlarm.hpp
        windows/PowerNapMainWindow.hpp
        windows/PowerNapProgressWindow.hpp
        windows/PowerNapSessionEndedWindow.hpp
        data/PowerNapListItem.hpp
        data/PowerNapStyle.hpp
        data/PowerNapCommon.hpp
        models/PowerNapModel.hpp

    PUBLIC
        include/application-bell-powernap/ApplicationBellPowerNap.hpp
)

target_link_libraries(application-bell-powernap
    PRIVATE
        app
        bell::app-common
        bell::app-main

    PUBLIC
        module-gui
)
