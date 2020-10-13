include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

target_sources(${PROJECT_NAME}
    PRIVATE
        ApplicationMeditation.cpp
        windows/MeditationListViewWindows.cpp
        windows/MeditationTimerWindow.cpp
        windows/MeditationWindow.cpp
        widgets/MeditationTimer.cpp
        widgets/MeditationListItemProviders.cpp
        widgets/MeditationListItems.cpp
        widgets/IntervalBox.cpp
        widgets/TimerSetter.cpp
    PUBLIC
        ApplicationMeditation.hpp
        data/Style.hpp
        windows/Names.hpp
        windows/MeditationListViewWindows.cpp
        windows/MeditationTimerWindow.hpp
        windows/MeditationWindow.hpp
        widgets/MeditationTimer.hpp
        widgets/MeditationListItemProviders.hpp
        widgets/MeditationListItems.hpp
        widgets/IntervalBox.hpp
        widgets/TimerSetter.hpp
)

