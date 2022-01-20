add_library(application-bell-onboarding STATIC)
add_library(bell::app-onboarding ALIAS application-bell-onboarding)

target_include_directories(application-bell-onboarding
    PRIVATE
        $<BUILD_INTERFACE:
            include/application-bell-onboarding
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-onboarding
    PRIVATE
        ApplicationBellOnBoarding.cpp
        windows/OnBoardingLanguageWindow.cpp
        windows/OnBoardingSettingsWindow.cpp
        windows/OnBoardingFinalizeWindow.cpp
        windows/OnBoardingOnOffWindow.cpp
        windows/OnBoardingInstructionPromptWindow.cpp
        windows/OnBoardingWelcomeWindow.cpp
        presenter/OnBoardingLanguageWindowPresenter.cpp
        presenter/OnBoardingFinalizeWindowPresenter.cpp

        presenter/OnBoardingLanguageWindowPresenter.hpp
        presenter/OnBoardingFinalizeWindowPresenter.hpp
        windows/OnBoardingLanguageWindow.hpp
        windows/OnBoardingOnOffWindow.hpp
        windows/OnBoardingSettingsWindow.hpp
        windows/OnBoardingInstructionPromptWindow.hpp
        windows/OnBoardingWelcomeWindow.hpp

    PUBLIC
        include/application-bell-onboarding/ApplicationBellOnBoarding.hpp
)

target_link_libraries(application-bell-onboarding
    PRIVATE
        app
        apps-common
        bell::app-settings

    PUBLIC
        module-gui
        bell::app-common
)
