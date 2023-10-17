target_sources( ${PROJECT_NAME}

    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationAlarmClock.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/AlarmClockMainWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/AlarmClockOptions.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/NewEditAlarmWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/CustomRepeatWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/models/AlarmsModel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/models/AlarmsRepository.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/models/NewEditAlarmModel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/models/CustomRepeatModel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmTimeItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmOptionsItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmSnoozeOptionsItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmMusicOptionsItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmRRuleOptionsItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/CustomCheckBoxWithLabel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/presenter/AlarmClockMainWindowPresenter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/presenter/AlarmClockEditWindowPresenter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/presenter/CustomRepeatWindowPresenter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/presenter/AlarmRRulePresenter.cpp"
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationAlarmClock.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmClockStyle.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/AlarmInternalListItem.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/data/AlarmsData.hpp"
        )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
