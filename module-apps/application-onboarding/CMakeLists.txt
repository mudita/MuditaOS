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
		"${CMAKE_CURRENT_LIST_DIR}/windows/StartConfigurationWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingLanguagesWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EULALicenseWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/EULALicenseWindowPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/OnBoardingFinalizeWindowPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/EULARepository.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ConfigurationSuccessfulDialogWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoConfigurationDialogWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateDialogWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SkipDialogWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingDateAndTimeWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingChangeDateAndTimeWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingSimSelectWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoSimSelectedDialogWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationOnBoarding.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/StartConfigurationWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingLanguagesWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/OnBoardingSwitchData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EULALicenseWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/EULALicenseWindowPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/OnBoardingFinalizeWindowPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/model/EULARepository.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ConfigurationSuccessfulDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoConfigurationDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SkipDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingDateAndTimeWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/OnBoardingSimSelectWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NoSimSelectedDialogWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/style/OnBoardingStyle.hpp"
)
