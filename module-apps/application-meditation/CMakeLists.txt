include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

target_sources(${PROJECT_NAME}
    PRIVATE
        ApplicationMeditation.cpp
        windows/MeditationListViewWindows.cpp
        windows/MeditationTimerWindow.cpp
        windows/MeditationWindow.cpp
        widgets/MeditationTimer.cpp
        widgets/MeditationModel.cpp
        widgets/MeditationListItems.cpp
        widgets/IntervalBox.cpp
        widgets/TimerSetter.cpp
    PUBLIC
        ApplicationMeditation.hpp
        data/Style.hpp
        data/MeditationTimerData.hpp
        data/OptionsData.hpp
        windows/Names.hpp
        windows/MeditationListViewWindows.hpp
        windows/MeditationTimerWindow.hpp
        windows/MeditationWindow.hpp
        widgets/MeditationTimer.hpp
        widgets/MeditationModel.hpp
        widgets/MeditationListItems.hpp
        widgets/IntervalBox.hpp
        widgets/TimerSetter.hpp
)

