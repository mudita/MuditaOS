add_library(application-bell-common STATIC)
add_library(bell::app-common ALIAS application-bell-common)

target_include_directories(application-bell-common
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/common>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-common
    PRIVATE
        src/AlarmModel.cpp
        src/TimeUtils.cpp
        src/popups/AlarmActivatedWindow.cpp
        src/SnoozeSettingsModel.cpp
        src/BellFinishedWindow.cpp
        src/BellSideListItemWithCallbacks.cpp
    PUBLIC
        include/common/BellFinishedWindow.hpp
        include/common/TimeUtils.hpp
        include/common/models/AbstractAlarmModel.hpp
        include/common/models/AlarmModel.hpp
        include/common/models/SnoozeSettingsModel.hpp
        include/common/popups/AlarmActivatedWindow.hpp
        include/common/widgets/BellSideListItemWithCallbacks.hpp
)

target_link_libraries(application-bell-common
    PUBLIC
        apps-common
        service-db

    PRIVATE
        bell::db
        module-gui
        service-time
        bell::db
        )
