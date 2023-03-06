add_library(application-bell-file-transfer)
add_library(bell::app-file-transfer ALIAS application-bell-file-transfer)

target_sources(application-bell-file-transfer
        PRIVATE
        ApplicationBellFileTransfer.cpp
        presenter/FileTransferPresenter.cpp
        windows/FileTransferWindow.cpp
        PRIVATE
        presenter/FileTransferPresenter.hpp
        windows/FileTransferWindow.hpp
        PUBLIC
        include/application-bell-file-transfer/ApplicationBellFileTransfer.hpp
        )

target_include_directories(application-bell-file-transfer
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/application-bell-file-transfer>
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_link_libraries(application-bell-file-transfer
        PRIVATE
        app
        bell::app-common
        bell::db
        date::date
        service-appmgr
        PUBLIC
        module-gui
        bell::app-common
        )
