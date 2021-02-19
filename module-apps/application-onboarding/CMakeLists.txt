include_directories( ${CMAKE_PROJECT_NAME}
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_sources(${PROJECT_NAME}

		PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationOnBoarding.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/StartConfigurationWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingLanguagesWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EULALicenseWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/EULALicenseWindowPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/EULARepository.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ConfigurationSuccessfulDialogWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoConfigurationDialogWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateDialogWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SkipDialogWindow.cpp"
		PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationOnBoarding.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/StartConfigurationWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingLanguagesWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/OnBoardingSwitchData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EULALicenseWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/EULALicenseWindowPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/EULARepository.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ConfigurationSuccessfulDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoConfigurationDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SkipDialogWindow.hpp"

		)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        service-db
)
