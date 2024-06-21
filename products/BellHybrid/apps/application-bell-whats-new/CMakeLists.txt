# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-bell-whats-new STATIC)
add_library(bell::app-whats-new ALIAS application-bell-whats-new)

target_include_directories(application-bell-whats-new
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            include/application-bell-whats-new
            data
            models
            presenter
            widgets
            windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-whats-new
    PRIVATE
        ApplicationWhatsNew.cpp

        models/WhatsNewFeaturesModel.cpp

        presenter/WhatsNewFeaturesPresenter.cpp
        presenter/WhatsNewMainPresenter.cpp

        widgets/WhatsNewFeaturesLayout.cpp

        windows/WhatsNewMainWindow.cpp
        windows/WhatsNewFeaturesWindow.cpp

    PUBLIC
        include/application-bell-whats-new/ApplicationWhatsNew.hpp
)

target_link_libraries(application-bell-whats-new
    PRIVATE
        bell::paths
        bell::db

    PUBLIC
        module-gui
)
