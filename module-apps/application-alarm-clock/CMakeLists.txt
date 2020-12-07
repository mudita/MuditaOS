target_sources( ${PROJECT_NAME}

        PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationAlarmClock.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/AlarmClockMainWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/models/AlarmsModel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/models/AlarmsRepository.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/presenter/AlarmClockMainWindowPresenter.cpp"
        PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationAlarmClock.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmClockStyle.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/data/AlarmsData.hpp"
        )
